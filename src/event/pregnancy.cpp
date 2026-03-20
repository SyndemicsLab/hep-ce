////////////////////////////////////////////////////////////////////////////////
// File: pregnancy.cpp                                                        //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/pregnancy_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<hepce::event::Event>
Pregnancy::Create(const data::Inputs &inputs, const std::string &log_name) {
    return std::make_unique<Pregnancy>(inputs, log_name);
}

// Execute
void Pregnancy::Execute(model::Person &person, const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    if (person.GetSex() == data::Sex::kMale || person.GetAge() < 180 ||
        CheckOldAge(person) || CheckPostpartumTime(person)) {
        return;
    }

    ProgressPostpartum(person);

    if (person.GetPregnancyDetails().pregnancy_state ==
        data::PregnancyState::kPregnant) {
        if (GetTimeSince(
                person,
                person.GetPregnancyDetails().time_of_pregnancy_change) >= 9) {
            AttemptHaveChild(person, sampler);
        }
        return;
    }

    double prob =
        _pregnancy_data[static_cast<int>(person.GetAge() / 12.0)].pregnant;
    if (sampler.GetDecision({1 - prob, prob})) {
        person.Impregnate();
    }
}

void Pregnancy::LoadData() {
    std::any storage = pregnancymap_t{};

    try {
        GetInputs().SelectFromDatabase(
            PregnancySQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                pregnancymap_t *temp = std::any_cast<pregnancymap_t>(&storage);
                struct pregnancy_probabilities d = {
                    stmt.getColumn(1).getDouble(),
                    stmt.getColumn(2).getDouble()};
                (*temp)[stmt.getColumn(0).getInt()] = d;
            },
            storage, {});
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting Pregnancy Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }

    _pregnancy_data = std::any_cast<pregnancymap_t>(storage);

    if (_pregnancy_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Pregnancy Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

// Private Methods
void Pregnancy::ProgressPostpartum(model::Person &person) const {
    auto state = person.GetPregnancyDetails().pregnancy_state;
    auto time = person.GetPregnancyDetails().time_of_pregnancy_change;
    switch (state) {
    case data::PregnancyState::kRestrictedPostpartum:
        person.SetPregnancyState(data::PregnancyState::kYearOnePostpartum);
        break;
    case data::PregnancyState::kYearOnePostpartum:
        if (GetTimeSince(person, time) >= 12) {
            person.SetPregnancyState(data::PregnancyState::kYearTwoPostpartum);
        }
        break;
    case data::PregnancyState::kYearTwoPostpartum:
        if (GetTimeSince(person, time) >= 24) {
            person.EndPostpartum();
        }
        break;
    default:
        break;
    }
}

void Pregnancy::AttemptHaveChild(model::Person &person,
                                 const model::Sampler &sampler) {
    if (CheckStillbirth(person, sampler)) {
        person.Stillbirth();
        return;
    }

    int num_births = GetNumberOfBirths(person, sampler);

    for (int child = 0; child < num_births; ++child) {
        bool tested = false;
        data::HCV hcv = data::HCV::kNone;
        if (person.GetHCVDetails().hcv == data::HCV::kChronic) {
            tested = DoChildrenGetTested(sampler);
            person.AddInfantExposure();
            if (DrawChildInfection(sampler)) {
                hcv = data::HCV::kChronic;
            }
        }
        person.Birth(MakeChild(hcv, tested));
    }
}

data::Child Pregnancy::MakeChild(const data::HCV &hcv, const bool &test) {
    data::Child child = {hcv, test};
    return child;
}
} // namespace event
} // namespace hepce
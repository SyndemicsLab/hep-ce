////////////////////////////////////////////////////////////////////////////////
// File: pregnancy.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/behavior/pregnancy.hpp>

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/pregnancy_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {

// Factory
std::unique_ptr<hepce::event::Event>
Pregnancy::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<PregnancyImpl>(model_data, log_name);
}

// Constructor
PregnancyImpl::PregnancyImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : EventBase(model_data, log_name) {
    LoadData(model_data);
}

// Execute
void PregnancyImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (person.GetSex() == data::Sex::kMale || person.GetAge() < 180 ||
        person.GetAge() > 540 ||
        (person.GetPregnancyDetails().pregnancy_state ==
             data::PregnancyState::kPostpartum &&
         GetTimeSince(person,
                      person.GetPregnancyDetails().time_of_pregnancy_change) <
             3)) {
        return;
    }

    if (person.GetPregnancyDetails().pregnancy_state ==
        data::PregnancyState::kPostpartum) {
        person.EndPostpartum();
    }

    if (person.GetPregnancyDetails().pregnancy_state ==
        data::PregnancyState::kPregnant) {
        if (GetTimeSince(
                person,
                person.GetPregnancyDetails().time_of_pregnancy_change) >= 9) {
            AttemptHaveChild(person, sampler);
        } else {
            AttemptHealthyMonth(person, sampler);
        }
    } else {
        double prob =
            _pregnancy_data[static_cast<int>(person.GetAge() / 12.0)].pregnant;
        if (sampler.GetDecision({1 - prob, prob})) {
            person.Impregnate();
        }
    }
    return;
}

void PregnancyImpl::LoadData(datamanagement::ModelData &model_data) {
    _multiple_delivery_probability = utils::GetDoubleFromConfig(
        "pregnancy.multiple_delivery_probability", model_data);

    _infant_hcv_tested_probability = utils::GetDoubleFromConfig(
        "pregnancy.infant_hcv_tested_probability", model_data);

    _vertical_hcv_transition_probability = utils::GetDoubleFromConfig(
        "pregnancy.vertical_hcv_transition_probability", model_data);

    std::any storage = _pregnancy_data;

    model_data.GetDBSource("inputs").Select(
        PregnancySQL(),
        [](std::any &storage, const SQLite::Statement &stmt) {
            struct pregnancy_probabilities d = {stmt.getColumn(1).getDouble(),
                                                stmt.getColumn(2).getDouble()};
            std::any_cast<pregnancymap_t>(storage)[stmt.getColumn(0).getInt()] =
                d;
        },
        storage);

    _pregnancy_data = std::any_cast<pregnancymap_t>(storage);

    if (_pregnancy_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Pregnancy Data is Empty...");
    }
}

// Private Methods
void PregnancyImpl::AttemptHaveChild(model::Person &person,
                                     model::Sampler &sampler) {
    if (CheckMiscarriage(person, sampler)) {
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

data::Child PregnancyImpl::MakeChild(data::HCV hcv, bool test) {
    data::Child child = {hcv, test};
    return child;
}
} // namespace behavior
} // namespace event
} // namespace hepce
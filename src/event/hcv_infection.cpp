////////////////////////////////////////////////////////////////////////////////
// File: hcv_infection.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/hcv_infection_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> HCVInfection::Create(const data::Inputs &inputs,
                                            const std::string &log_name) {
    return std::make_unique<HCVInfection>(inputs, log_name);
}

// Execute
void HCVInfection::Execute(model::Person &person,
                           const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // Acute cases progress to chronic after 6 consecutive months of
    // infection
    if (person.GetHCVDetails().hcv == data::HCV::kAcute &&
        GetTimeSince(person, person.GetHCVDetails().time_changed) >= 6) {
        person.SetHCV(data::HCV::kChronic);
    }

    // If already infected, skip
    if (person.GetHCVDetails().hcv != data::HCV::kNone) {
        return;
    }

    // draw new infection probability
    std::vector<double> prob = GetInfectionProbability(person);
    // decide whether person is infected; if value == 0, infect
    if (sampler.GetDecision(prob) == 0) {
        person.InfectHCV();
        // decide whether hcv is genotype three
        if (sampler.GetDecision({_gt3_prob}) == 0) {
            person.SetGenotypeThree(true);
        }
    }
}

void HCVInfection::LoadData() {
    std::string error;
    std::any storage = incidencemap_t{};
    try {
        GetInputs().SelectFromDatabase(IncidenceSQL(), CallbackInfection,
                                       storage, {});
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting HCV Infection Incidence Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _infection_data = std::any_cast<incidencemap_t>(storage);
    if (_infection_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Incidence Table is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

// Private Methods
std::vector<double>
HCVInfection::GetInfectionProbability(const model::Person &person) {
    if (_infection_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Infection Incidence Data is Empty. Returning "
                                 "Probability of 0.0...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        return {0.0};
    }

    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int drug_behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
    double incidence = _infection_data[tup];

    return {incidence};
}

} // namespace event
} // namespace hepce
////////////////////////////////////////////////////////////////////////////////
// File: hcv_clearance.cpp                                                    //
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
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/hcv_clearance_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> HCVClearance::Create(const data::Inputs &inputs,
                                            const std::string &log_name) {
    return std::make_unique<HCVClearance>(inputs, log_name);
}

// Execute
void HCVClearance::Execute(model::Person &person,
                           const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // if person isn't infected or is chronic, nothing to do
    // Also skip if person is already on treatment since we want this to
    // count as SVR
    if (person.GetHCVDetails().hcv != data::HCV::kAcute ||
        person.GetTreatmentDetails(data::InfectionType::kHcv)
            .initiated_treatment) {
        return;
    }
    if (sampler.GetDecision({_probability, 1 - _probability}) == 0) {
        person.ClearHCV(true);
    }
}

void HCVClearance::LoadData() {
    _probability =
        utils::GetDoubleFromConfig("infection.clearance_prob", GetInputs());

    if (_probability == -1) {
        hepce::utils::LogInfo(
            GetLogName(), "Infection Clearance Probability is not found. "
                          "setting to default value of 25\% over 6 months.");
        _probability =
            utils::RateToProbability(utils::ProbabilityToRate(0.25, 6));
    }
}
} // namespace event
} // namespace hepce
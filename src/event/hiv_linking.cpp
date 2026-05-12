////////////////////////////////////////////////////////////////////////////////
// File: hiv_linking.cpp                                                      //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "internals/hiv_linking_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> HIVLinking::Create(const data::Inputs &inputs,
                                          const std::string &log_name) {
    return std::make_unique<HIVLinking>(inputs, log_name);
}

void HIVLinking::LoadData() {
    SetCostCategory(model::CostCategory::kHiv);
    SetLinkingStratifiedByPregnancy(
        utils::FindInEventList("pregnancy", GetInputs()));
    LoadLinkingData();

    SetInterventionCost(utils::GetDoubleFromConfig(
        "hiv_linking.intervention_cost", GetInputs()));
    SetFalsePositiveCost(utils::GetDoubleFromConfig(
        "hiv_linking.false_positive_test_cost", GetInputs()));
    SetScalingCoefficient(utils::GetDoubleFromConfig(
        "hiv_linking.scaling_coefficient", GetInputs()));
    SetRecentScreenCutoff(utils::GetIntFromConfig(
        "hiv_linking.recent_screen_cutoff", GetInputs()));
}
} // namespace event
} // namespace hepce
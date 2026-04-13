////////////////////////////////////////////////////////////////////////////////
// File: hcv_linking.cpp                                                      //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                    //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Library Includes
#include <hepce/utils/config.hpp>

// Local Includes
#include "internals/hcv_linking_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> HCVLinking::Create(const data::Inputs &inputs,
                                          const std::string &log_name) {
    return std::make_unique<HCVLinking>(inputs, log_name);
}

void HCVLinking::LoadData() {
    SetCostCategory(model::CostCategory::kLinking);
    SetLinkingStratifiedByPregnancy(
        utils::FindInEventList("Pregnancy", GetInputs()));
    LoadLinkingData();
    SetInterventionCost(
        utils::GetDoubleFromConfig("linking.intervention_cost", GetInputs()));
    SetFalsePositiveCost(utils::GetDoubleFromConfig(
        "linking.false_positive_test_cost", GetInputs()));
    SetScalingType(
        utils::GetStringFromConfig("linking.scaling_type", GetInputs()));
    if (GetScalingType() == "exponential") {
        return;
    }
    DetermineRecentScreenCutoff(
        utils::GetIntFromConfig("linking.recent_screen_cutoff", GetInputs()));
    try {
        SetScalingCoefficient(utils::GetDoubleFromConfig(
            "linking.scaling_coefficient", GetInputs()));
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Invalid argument: linking.scaling_coefficient -- " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
    }
}
} // namespace event
} // namespace hepce
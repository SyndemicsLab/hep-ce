////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/hiv/linking.hpp>

#include "hiv/internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
// Factory
std::unique_ptr<hepce::event::Event>
Linking::Create(datamanagement::ModelData &model_data,
                const std::string &log_name) {
    return std::make_unique<LinkingImpl>(model_data, log_name);
}

// Constructor
LinkingImpl::LinkingImpl(datamanagement::ModelData &model_data,
                         const std::string &log_name = "console")
    : LinkingBase(model_data, log_name) {
    SetEventCostCategory(model::CostCategory::kHiv);
    SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(model_data));
    LoadLinkingData(model_data);

    SetInterventionCost(utils::GetDoubleFromConfig(
        "hiv_linking.intervention_cost", model_data));
    SetFalsePositiveCost(utils::GetDoubleFromConfig(
        "hiv_linking.false_positive_test_cost", model_data));
    SetRecentScreenMultiplier(utils::GetDoubleFromConfig(
        "hiv_linking.recent_screen_multiplier", model_data));
    SetRecentScreenCutoff(utils::GetIntFromConfig(
        "hiv_linking.recent_screen_cutoff", model_data));
}
} // namespace hiv
} // namespace event
} // namespace hepce
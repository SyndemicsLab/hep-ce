////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/hcv/linking.hpp>

// Library Includes
#include <hepce/utils/config.hpp>

// Local Includes
#include "internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {

// Factory
std::unique_ptr<hepce::event::Event>
hcv::Linking::Create(datamanagement::ModelData &model_data,
                     const std::string &log_name) {
    return std::make_unique<hcv::LinkingImpl>(model_data, log_name);
}

// Constructor
LinkingImpl::LinkingImpl(datamanagement::ModelData &model_data,
                         const std::string &log_name)
    : LinkingBase(model_data, log_name) {
    LoadData(model_data);
}

void LinkingImpl::LoadData(datamanagement::ModelData &model_data) {
    SetEventCostCategory(model::CostCategory::kLinking);
    SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(model_data));
    LoadLinkingData(model_data);
    SetInterventionCost(
        utils::GetDoubleFromConfig("linking.intervention_cost", model_data));
    SetFalsePositiveCost(utils::GetDoubleFromConfig(
        "linking.false_positive_test_cost", model_data));
    SetRecentScreenMultiplier(utils::GetDoubleFromConfig(
        "linking.recent_screen_multiplier", model_data));
    SetRecentScreenCutoff(
        utils::GetIntFromConfig("linking.recent_screen_cutoff", model_data));
}
} // namespace hcv
} // namespace event
} // namespace hepce
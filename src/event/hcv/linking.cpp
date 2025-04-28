////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/hcv/linking.hpp>

#include "hcv/internals/linking_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace hcv {
std::unique_ptr<hepce::event::Event>
hcv::Linking::Create(datamanagement::ModelData &model_data,
                     const std::string &log_name) {
    return std::make_unique<hcv::LinkingImpl>(model_data, log_name);
}

LinkingImpl::LinkingImpl(datamanagement::ModelData &model_data,
                         const std::string &log_name) {
    SetDiscount(
        utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
    SetCostCategory(model::CostCategory::kLinking);
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
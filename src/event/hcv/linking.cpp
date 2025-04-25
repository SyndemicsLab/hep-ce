////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
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
hcv::Linking::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                     const std::string &log_name) {
    return std::make_unique<hcv::LinkingImpl>(dm, log_name);
}

LinkingImpl::LinkingImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                         const std::string &log_name) {
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    SetCostCategory(model::CostCategory::kLinking);
    SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(dm));
    LoadLinkingData(dm);
    SetInterventionCost(
        utils::GetDoubleFromConfig("linking.intervention_cost", dm));
    SetFalsePositiveCost(
        utils::GetDoubleFromConfig("linking.false_positive_test_cost", dm));
    SetRecentScreenMultiplier(
        utils::GetDoubleFromConfig("linking.recent_screen_multiplier", dm));
    SetRecentScreenCutoff(
        utils::GetIntFromConfig("linking.recent_screen_cutoff", dm));
}
} // namespace hcv
} // namespace event
} // namespace hepce
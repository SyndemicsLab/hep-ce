////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-05                                                  //
// Modified By: Andrew Clark                                                  //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/hiv/linking.hpp>

// Local Includes
#include "internals/linking_internals.hpp"

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
                         const std::string &log_name)
    : LinkingBase(model_data, log_name) {
    LoadData(model_data);
}

void LinkingImpl::LoadData(datamanagement::ModelData &model_data) {
    SetEventCostCategory(model::CostCategory::kHiv);
    SetLinkingStratifiedByPregnancy(
        utils::FindInEventList("pregnancy", model_data));
    LoadLinkingData(model_data);

    SetInterventionCost(utils::GetDoubleFromConfig(
        "hiv_linking.intervention_cost", model_data));
    SetFalsePositiveCost(utils::GetDoubleFromConfig(
        "hiv_linking.false_positive_test_cost", model_data));
    SetScalingCoefficient(utils::GetDoubleFromConfig(
        "hiv_linking.scaling_coefficient", model_data));
    SetRecentScreenCutoff(utils::GetIntFromConfig(
        "hiv_linking.recent_screen_cutoff", model_data));
}
} // namespace hiv
} // namespace event
} // namespace hepce
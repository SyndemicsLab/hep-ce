////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                    //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-05                                                  //
// Modified By: Andrew Clark                                                  //
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
    SetLinkingStratifiedByPregnancy(
        utils::FindInEventList("Pregnancy", model_data));
    LoadLinkingData(model_data);
    SetInterventionCost(
        utils::GetDoubleFromConfig("linking.intervention_cost", model_data));
    SetFalsePositiveCost(utils::GetDoubleFromConfig(
        "linking.false_positive_test_cost", model_data));
    SetScalingType(
        utils::GetStringFromConfig("linking.scaling_type", model_data));
    if (GetScalingType() == "multiplier" || GetScalingType() == "sigmoidal") {
        SetRecentScreenCutoff(utils::GetIntFromConfig(
            "linking.recent_screen_cutoff", model_data));
        try {
            SetScalingCoefficient(utils::GetDoubleFromConfig(
                "linking.scaling_coefficient", model_data));
        } catch(std::exception &e) {
            std::stringstream msg;
            msg << "Invalid argument: linking.scaling_coefficient -- " << e.what();
            hepce::utils::LogError(GetLogName(), msg.str());
        }
    }
}
} // namespace hcv
} // namespace event
} // namespace hepce
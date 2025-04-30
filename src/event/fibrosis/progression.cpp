////////////////////////////////////////////////////////////////////////////////
// File: progression.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/fibrosis/progression.hpp>

#include "internals/progression_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace fibrosis {

// Factory
std::unique_ptr<hepce::event::Event>
Progression::Create(datamanagement::ModelData &model_data,
                    const std::string &log_name) {
    return std::make_unique<ProgressionImpl>(model_data, log_name);
}

// Constructor
ProgressionImpl::ProgressionImpl(datamanagement::ModelData &model_data,
                                 const std::string &log_name)
    : EventBase(model_data, log_name) {
    SetEventUtilityCategory(model::UtilityCategory::kLiver);
    SetEventCostCategory(model::CostCategory::kLiver);

    _probabilities = {utils::GetDoubleFromConfig("fibrosis.f01", model_data),
                      utils::GetDoubleFromConfig("fibrosis.f12", model_data),
                      utils::GetDoubleFromConfig("fibrosis.f23", model_data),
                      utils::GetDoubleFromConfig("fibrosis.f34", model_data),
                      utils::GetDoubleFromConfig("fibrosis.f4d", model_data)};

    _add_cost_data = utils::GetBoolFromConfig(
        "fibrosis.add_cost_only_if_identified", model_data);

    GetProgressionData(model_data);
}

// Execute
void ProgressionImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // can only progress in fibrosis state if actively infected with HCV
    if (person.GetHCVDetails().hcv == data::HCV::kNone) {
        return;
    }
    // 1. Get current fibrosis status
    data::FibrosisState fs = person.GetHCVDetails().fibrosis_state;
    // 2. Get the transition probability
    std::vector<double> prob = GetTransitionProbability(fs);
    // 3. Draw whether the person's fibrosis state progresses (0
    // progresses)
    (sampler.GetDecision(prob) == 0) ? ++fs : fs;

    if (fs != person.GetHCVDetails().fibrosis_state) {
        // 4. Apply the result state
        person.UpdateTrueFibrosis(fs);
    }

    // insert Person's liver-related disease cost (taking the highest
    // fibrosis state) only if the person is identified as infected
    if (_add_cost_data &&
        person.GetScreeningDetails(data::InfectionType::kHcv).identified) {
        AddProgressionCost(person);
    }
    AddProgressionUtility(person);
}

// Private Methods
const std::vector<double>
ProgressionImpl::GetTransitionProbability(const data::FibrosisState &fs) const {
    switch (fs) {
    case data::FibrosisState::kF0:
        return {_probabilities.f0_to_1, 1 - _probabilities.f0_to_1};
    case data::FibrosisState::kF1:
        return {_probabilities.f1_to_2, 1 - _probabilities.f1_to_2};
    case data::FibrosisState::kF2:
        return {_probabilities.f2_to_3, 1 - _probabilities.f2_to_3};
    case data::FibrosisState::kF3:
        return {_probabilities.f3_to_4, 1 - _probabilities.f3_to_4};
    case data::FibrosisState::kF4:
        return {_probabilities.f4_to_d, 1 - _probabilities.f4_to_d};
    default:
        return {0.0, 1.0};
    }
}
} // namespace fibrosis
} // namespace event
} // namespace hepce
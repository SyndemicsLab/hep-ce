////////////////////////////////////////////////////////////////////////////////
// File: progression.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
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
std::unique_ptr<hepce::event::Event>
Progression::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                    const std::string &log_name) {
    return std::make_unique<ProgressionImpl>(dm, log_name);
}

ProgressionImpl::ProgressionImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name = "console") {
    SetUtilityCategory(model::UtilityCategory::kLiver);
    SetCostCategory(model::CostCategory::kLiver);
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));

    _probabilities = {utils::GetDoubleFromConfig("fibrosis.f01", dm),
                      utils::GetDoubleFromConfig("fibrosis.f12", dm),
                      utils::GetDoubleFromConfig("fibrosis.f23", dm),
                      utils::GetDoubleFromConfig("fibrosis.f34", dm),
                      utils::GetDoubleFromConfig("fibrosis.f4d", dm)};

    _add_cost_data =
        utils::GetBoolFromConfig("fibrosis.add_cost_only_if_identified", dm);

    GetProgressionData(dm);
}

int ProgressionImpl::Execute(
    model::Person &person, std::shared_ptr<datamanagement::DataManagerBase> dm,
    model::Sampler &sampler) {
    // can only progress in fibrosis state if actively infected with HCV
    if (person.GetHCV() == data::HCV::NONE) {
        return;
    }
    // 1. Get current fibrosis status
    data::FibrosisState fs = person.GetTrueFibrosisState();
    // 2. Get the transition probability
    std::vector<double> prob = GetTransitionProbability(fs);
    // 3. Draw whether the person's fibrosis state progresses (0
    // progresses)
    (sampler.GetDecision(prob) == 0) ? ++fs : fs;

    if (fs != person.GetTrueFibrosisState()) {
        // 4. Apply the result state
        person.UpdateTrueFibrosis(fs);
    }

    // insert Person's liver-related disease cost (taking the highest
    // fibrosis state) only if the person is identified as infected
    if (_add_cost_data && person.IsIdentifiedAsInfected()) {
        AddProgressionCost(person);
    }
    AddProgressionUtility(person);
}

const std::vector<double>
ProgressionImpl::GetTransitionProbability(const data::FibrosisState &fs) const {
    switch (fs) {
    case data::FibrosisState::F0:
        return {_probabilities.f0_to_1, 1 - _probabilities.f0_to_1};
    case data::FibrosisState::F1:
        return {_probabilities.f1_to_2, 1 - _probabilities.f1_to_2};
    case data::FibrosisState::F2:
        return {_probabilities.f2_to_3, 1 - _probabilities.f2_to_3};
    case data::FibrosisState::F3:
        return {_probabilities.f3_to_4, 1 - _probabilities.f3_to_4};
    case data::FibrosisState::F4:
        return {_probabilities.f4_to_d, 1 - _probabilities.f4_to_d};
    default:
        return {0.0, 1.0};
    }
}
} // namespace fibrosis
} // namespace event
} // namespace hepce
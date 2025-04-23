////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes.cpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/behavior/behavior_changes.hpp>

#include "behavior/internals/behavior_changes_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace behavior {
BehaviorChangesImpl::BehaviorChangesImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name = "console") {
    SetCostCategory(model::CostCategory::kBehavior);
    SetUtilityCategory(model::UtilityCategory::kBehavior);
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    LoadCostData(dm);
    LoadBehaviorData(dm);
}

int BehaviorChangesImpl::Execute(
    model::Person &person, std::shared_ptr<datamanagement::DataManagerBase> dm,
    model::Sampler &sampler) {

    // Typical Behavior Change
    // 1. Generate the transition probabilities based on the starting
    // state
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int moud = static_cast<int>(person.GetMoudState());
    int behavior = static_cast<int>(person.GetBehavior());
    utils::tuple_4i tup = std::make_tuple(age_years, gender, moud, behavior);
    std::vector<double> probs = {behavior_data[tup].never,
                                 behavior_data[tup].fni, behavior_data[tup].fi,
                                 behavior_data[tup].ni, behavior_data[tup].in};

    // 2. Draw a behavior state to be transitioned to
    int res = sampler.GetDecision(probs);
    if (res >= static_cast<int>(data::Behavior::COUNT)) {
        // spdlog::get("main")->error("Behavior Classification Decision returned "
        //                            "value outside bounds");
        return;
    }

    // 3. If the drawn state differs from the current state, change the
    // bools in BehaviorState to match
    person.SetBehavior(static_cast<data::Behavior>(res));

    // Insert person's behavior cost
    CalculateCostAndUtility(person);
}

int BehaviorChangesImpl::LoadCostData(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string error;
    int rc = dm->SelectCustomCallback(CostSQL(), this->callback_costs,
                                      &cost_data, error);
    if (rc != 0) {
        // spdlog::get("main")->error(
        //     "Error extracting Behavior Change Data from background "
        //     "costs and "
        //     "background behaviors! Error Message: {}",
        //     error);
    }
    if (cost_data.empty()) {
        // spdlog::get("main")->warn(
        //     "No Background Cost found for Behavior Changes!");
    }
    return rc;
}

int BehaviorChangesImpl::LoadBehaviorData(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string error;
    int rc = dm->SelectCustomCallback(TransitionSQL(), this->callback_trans,
                                      &behavior_data, error);
    if (rc != 0) {
        // spdlog::get("main")->error("Error extracting Behavior Change "
        //                            "Transition Data! Error Message: {}",
        //                            error);
    }
    if (behavior_data.empty()) {
        // spdlog::get("main")->warn(
        //     "No Transition Data found for Behavior Changes!");
    }
    return rc;
}

void BehaviorChangesImpl::CalculateCostAndUtility(model::Person &person) {
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehavior());
    utils::tuple_2i tup = std::make_tuple(gender, behavior);

    SetCost(cost_data[tup].cost);
    AddEventCost(person);

    SetUtil(cost_data[tup].util);
    AddUtility(person);
}

std::unique_ptr<hepce::event::Event>
BehaviorChanges::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                        const std::string &log_name) {
    return std::make_unique<BehaviorChangesImpl>(dm, log_name);
}

} // namespace behavior
} // namespace event
} // namespace hepce
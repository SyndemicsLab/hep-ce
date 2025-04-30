////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes.cpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/behavior/behavior_changes.hpp>

// Library Includes
#include <hepce/utils/config.hpp>

// Local Includes
#include "internals/behavior_changes_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {

// Factory
std::unique_ptr<hepce::event::Event>
BehaviorChanges::Create(datamanagement::ModelData &model_data,
                        const std::string &log_name) {
    return std::make_unique<BehaviorChangesImpl>(model_data, log_name);
}

// Constructor
BehaviorChangesImpl::BehaviorChangesImpl(
    datamanagement::ModelData &model_data,
    const std::string &log_name = "console")
    : EventBase(model_data, log_name) {
    SetEventCostCategory(model::CostCategory::kBehavior);
    SetEventUtilityCategory(model::UtilityCategory::kBehavior);
    LoadCostData(model_data);
    LoadBehaviorData(model_data);
}

// Execute
int BehaviorChangesImpl::Execute(model::Person &person,
                                 model::Sampler &sampler) {

    // Typical Behavior Change
    // 1. Generate the transition probabilities based on the starting
    // state
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int moud = static_cast<int>(person.GetMoudDetails().moud_state);
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_4i tup = std::make_tuple(age_years, gender, moud, behavior);
    std::vector<double> probs = {
        _behavior_data[tup].never, _behavior_data[tup].fni,
        _behavior_data[tup].fi, _behavior_data[tup].ni, _behavior_data[tup].in};

    // 2. Draw a behavior state to be transitioned to
    int res = sampler.GetDecision(probs);
    if (res >= static_cast<int>(data::Behavior::kCount)) {
        // Log Error
        return;
    }

    // 3. If the drawn state differs from the current state, change the
    // bools in BehaviorState to match
    person.SetBehavior(static_cast<data::Behavior>(res));

    // Insert person's behavior cost
    CalculateCostAndUtility(person);
}

// Private Methods
int BehaviorChangesImpl::LoadCostData(datamanagement::ModelData &model_data) {
    std::any storage = _cost_data;

    model_data.GetDBSource("inputs").Select(
        CostSQL(),
        [](std::any &storage, const SQLite::Statement &stmt) {
            utils::tuple_2i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                                  stmt.getColumn(1).getInt());
            data::CostUtil cu = {stmt.getColumn(2).getDouble(),
                                 stmt.getColumn(3).getDouble()};
            std::any_cast<costmap_t>(storage)[tup] = cu;
        },
        storage);
    _cost_data = std::any_cast<costmap_t>(storage);
    if (_cost_data.empty()) {
        // Warn Empty
    }
    return 0;
}

int BehaviorChangesImpl::LoadBehaviorData(
    datamanagement::ModelData &model_data) {
    std::any storage = _behavior_data;

    model_data.GetDBSource("inputs").Select(
        TransitionSQL(),
        [](std::any &storage, const SQLite::Statement &stmt) {
            utils::tuple_4i tup = std::make_tuple(
                stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
                stmt.getColumn(2).getInt(), stmt.getColumn(3).getInt());
            struct behavior_transitions behavior = {
                stmt.getColumn(4).getDouble(), stmt.getColumn(5).getDouble(),
                stmt.getColumn(6).getDouble(), stmt.getColumn(7).getDouble(),
                stmt.getColumn(8).getDouble()};
            std::any_cast<behaviormap_t>(storage)[tup] = behavior;
        },
        storage);

    if (_behavior_data.empty()) {
        // Warn Empty
    }
    return 0;
}

void BehaviorChangesImpl::CalculateCostAndUtility(model::Person &person) {
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_2i tup = std::make_tuple(gender, behavior);

    SetEventCost(_cost_data[tup].cost);
    AddEventCost(person);

    SetEventUtility(_cost_data[tup].util);
    AddEventUtility(person);
}
} // namespace behavior
} // namespace event
} // namespace hepce
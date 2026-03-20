////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes.cpp                                                 //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <memory>

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/behavior_changes_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> BehaviorChanges::Create(const data::Inputs &inputs,
                                               const std::string &log_name) {
    return std::make_unique<BehaviorChanges>(inputs, log_name);
}

// Execute
void BehaviorChanges::Execute(model::Person &person,
                              const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }

    // Typical Behavior Change
    // 1. Generate the transition probabilities based on the starting
    // state
    auto probs = GetBehaviorTransitionProbabilities(person);

    auto behavior = person.GetBehaviorDetails().behavior;

    if (behavior == data::Behavior::kFormerInjection ||
        behavior == data::Behavior::kFormerNoninjection) {
        double decay_value =
            GetExponentialChange(person.GetCurrentTimestep() -
                                 person.GetBehaviorDetails().time_last_active);
        ApplyDecayToRelapseProbabilities(probs, decay_value, behavior);
    }

    // 2. Draw a behavior state to be transitioned to
    int res = sampler.GetDecision(probs);
    if (res >= static_cast<int>(data::Behavior::kCount)) {
        hepce::utils::LogError(
            GetLogName(),
            "Invalid Decision returned during the Behavior Change Event!");
        return;
    }

    // 3. If the drawn state differs from the current state, change the
    // bools in BehaviorState to match
    person.SetBehavior(static_cast<data::Behavior>(res));

    // Insert person's behavior cost
    CalculateCostAndUtility(person);
}

// Private Methods
std::vector<double> BehaviorChanges::GetBehaviorTransitionProbabilities(
    const model::Person &person) const {
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    int moud = static_cast<int>(person.GetMoudDetails().moud_state);
    utils::tuple_4i tup = std::make_tuple(age_years, gender, behavior, moud);
    std::vector<double> probs;
    try {
        probs = {_behavior_data.at(tup).never, _behavior_data.at(tup).fni,
                 _behavior_data.at(tup).fi, _behavior_data.at(tup).ni,
                 _behavior_data.at(tup).in};
    } catch (std::out_of_range &e) {
        std::stringstream msg;
        msg << "Behavior Transition Probabilities are not found for the person "
               "details (age, Sex, Behavior, MOUD): "
            << age_years << ", " << person.GetSex() << ", "
            << person.GetBehaviorDetails().behavior << ", "
            << person.GetMoudDetails().moud_state
            << "! Returning guaranteed injection use.";
        hepce::utils::LogError(GetLogName(), msg.str());
        return {0.0, 0.0, 0.0, 0.0, 1.0};
    }
    return probs;
}

void BehaviorChanges::LoadCostData() {
    std::any storage = costmap_t{};

    try {
        GetInputs().SelectFromDatabase(
            CostSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                costmap_t *temp = std::any_cast<costmap_t>(&storage);
                utils::tuple_2i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt());
                data::CostUtil cu = {stmt.getColumn(2).getDouble(),
                                     stmt.getColumn(3).getDouble()};
                (*temp)[tup] = cu;
            },
            storage, {});
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting cost data in behavior changes: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _cost_data = std::any_cast<costmap_t>(storage);
    if (_cost_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Behavior Changes Cost Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

void BehaviorChanges::LoadBehaviorData() {
    std::any storage = _behavior_data;
    try {
        GetInputs().SelectFromDatabase(
            TransitionSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                behaviormap_t *temp = std::any_cast<behaviormap_t>(&storage);
                utils::tuple_4i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
                    stmt.getColumn(2).getInt(), stmt.getColumn(3).getInt());
                struct behavior_transitions behavior = {
                    stmt.getColumn(4).getDouble(),
                    stmt.getColumn(5).getDouble(),
                    stmt.getColumn(6).getDouble(),
                    stmt.getColumn(7).getDouble(),
                    stmt.getColumn(8).getDouble()};

                (*temp)[tup] = behavior;
            },
            storage, {});
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting Behavior Transition Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _behavior_data = std::any_cast<behaviormap_t>(storage);
    if (_behavior_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Behavior Data Transitions Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

void BehaviorChanges::ApplyDecayToRelapseProbabilities(
    std::vector<double> &probs, double decay_value,
    data::Behavior current_behavior) const {

    std::unordered_map<data::Behavior, data::Behavior> relapse_behaviors = {
        {data::Behavior::kFormerInjection, data::Behavior::kInjection},
        {data::Behavior::kFormerNoninjection, data::Behavior::kNoninjection}};

    int current_idx = static_cast<int>(current_behavior);

    int relapse_idx = static_cast<int>(relapse_behaviors[current_behavior]);
    double temp = probs[relapse_idx];
    probs[relapse_idx] = utils::RateToProbability(
        utils::ProbabilityToRate(probs[relapse_idx]) * decay_value);
    // the change in relapse is applied to retention
    probs[current_idx] += temp - probs[relapse_idx];
}

void BehaviorChanges::CalculateCostAndUtility(model::Person &person) const {
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_2i tup = std::make_tuple(gender, behavior);

    auto cu = _cost_data.at(tup);
    AddEventCost(person, cu.cost);
    AddEventUtility(person, cu.util);
}
} // namespace event
} // namespace hepce
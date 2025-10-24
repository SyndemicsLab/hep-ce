////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes.cpp                                                 //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/behavior/behavior_changes.hpp>

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/logging.hpp>

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
BehaviorChangesImpl::BehaviorChangesImpl(datamanagement::ModelData &model_data,
                                         const std::string &log_name)
    : EventBase(model_data, log_name),
      _first_year_relapse_rate(utils::GetDoubleFromConfig(
          "behavior.first_year_relapse_rate", model_data)),
      _later_years_relapse_rate(utils::GetDoubleFromConfig(
          "behavior.later_years_relapse_rate", model_data)) {
    SetEventCostCategory(model::CostCategory::kBehavior);
    SetEventUtilityCategory(model::UtilityCategory::kBehavior);
    LoadData(model_data);
}

// Execute
void BehaviorChangesImpl::Execute(model::Person &person,
                                  model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }

    // Typical Behavior Change
    // 1. Generate the transition probabilities based on the starting
    // state
    auto probs = GetBehaviorTransitionProbabilities(person);

    auto behavior = person.GetBehaviorDetails().behavior;

    if (behavior == data::Behavior::kFormerInjection) {
        ApplyDecayToRelapseProbabilities(
            probs, person.GetBehaviorDetails().time_last_active,
            data::Behavior::kFormerInjection, data::Behavior::kInjection);
    } else if (behavior == data::Behavior::kFormerNoninjection) {
        ApplyDecayToRelapseProbabilities(
            probs, person.GetBehaviorDetails().time_last_active,
            data::Behavior::kFormerNoninjection, data::Behavior::kNoninjection);
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

void BehaviorChangesImpl::LoadData(datamanagement::ModelData &model_data) {
    LoadCostData(model_data);
    LoadBehaviorData(model_data);
}

// Private Methods
std::vector<double> BehaviorChangesImpl::GetBehaviorTransitionProbabilities(
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

void BehaviorChangesImpl::LoadCostData(datamanagement::ModelData &model_data) {
    std::any storage = costmap_t{};

    try {
        model_data.GetDBSource("inputs").Select(
            CostSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                costmap_t *temp = std::any_cast<costmap_t>(&storage);
                utils::tuple_2i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt());
                data::CostUtil cu = {stmt.getColumn(2).getDouble(),
                                     stmt.getColumn(3).getDouble()};
                (*temp)[tup] = cu;
            },
            storage);
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

void BehaviorChangesImpl::LoadBehaviorData(
    datamanagement::ModelData &model_data) {
    std::any storage = _behavior_data;
    try {
        model_data.GetDBSource("inputs").Select(
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
            storage);
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

void BehaviorChangesImpl::ApplyDecayToRelapseProbabilities(
    std::vector<double> &probs, int time_since_quit,
    data::Behavior current_behavior, data::Behavior relapse_behavior) const {
    // Magic Number 12 corresponds to 12 months in a year
    double relapse_rate = (time_since_quit < 12) ? _first_year_relapse_rate
                                                 : _later_years_relapse_rate;

    int current_idx = static_cast<int>(current_behavior);
    int relapse_idx = static_cast<int>(relapse_behavior);

    double exponential_decay =
        std::exp(-relapse_rate * static_cast<double>(time_since_quit));

    probs[relapse_idx] = utils::RateToProbability(
        utils::ProbabilityToRate(probs[current_idx]) * exponential_decay);
    probs[current_idx] = 1 - probs[relapse_idx];
}

void BehaviorChangesImpl::CalculateCostAndUtility(model::Person &person) {
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_2i tup = std::make_tuple(gender, behavior);

    AddEventCost(person, _cost_data[tup].cost);
    AddEventUtility(person, _cost_data[tup].util);
}
} // namespace behavior
} // namespace event
} // namespace hepce
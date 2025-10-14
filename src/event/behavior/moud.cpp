////////////////////////////////////////////////////////////////////////////////
// File: moud.cpp                                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-05-08                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/behavior/moud.hpp>

#include <hepce/utils/logging.hpp>

#include "internals/moud_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
// Factory
std::unique_ptr<Event> Moud::Create(datamanagement::ModelData &model_data,
                                    const std::string &log_name) {
    return std::make_unique<MoudImpl>(model_data, log_name);
}

MoudImpl::MoudImpl(datamanagement::ModelData &model_data,
                   const std::string &log_name)
    : EventBase(model_data, log_name) {
    LoadData(model_data);
    SetEventCostCategory(model::CostCategory::kBehavior);
    SetEventUtilityCategory(model::UtilityCategory::kBehavior);
}

void MoudImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (!ValidExecute(person) || !HistoryOfOud(person)) {
        return;
    }
    data::MOUD current_moud = person.GetMoudDetails().moud_state;

    // If on Post-Treatment transition and quick stop
    if (current_moud == data::MOUD::kPost) {
        person.TransitionMOUD();
        CalculateCostAndUtility(person);
        return;
    }

    // Draw Transition Probability, increment months or start/stop
    int res = sampler.GetDecision(GetMoudTransitionProbability(person));

    // If Current->Post OR None->Current
    if ((current_moud == data::MOUD::kCurrent && res == 2) ||
        (current_moud == data::MOUD::kNone && res == 1)) {
        person.TransitionMOUD();
    }
    // Insert person's behavior cost
    CalculateCostAndUtility(person);
}

void MoudImpl::LoadData(datamanagement::ModelData &model_data) {
    LoadMOUDData(model_data);
    LoadCostData(model_data);
}

void MoudImpl::LoadMOUDData(datamanagement::ModelData &model_data) {
    std::any storage = _moud_data;
    try {
        model_data.GetDBSource("inputs").Select(
            TransitionSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                moudmap_t *temp = std::any_cast<moudmap_t>(&storage);
                utils::tuple_4i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
                    stmt.getColumn(2).getInt(), stmt.getColumn(3).getInt());
                struct moud_transitions behavior = {
                    stmt.getColumn(4).getDouble(),
                    stmt.getColumn(5).getDouble(),
                    stmt.getColumn(6).getDouble()};

                (*temp)[tup] = behavior;
            },
            storage);
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting MOUD Transition Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _moud_data = std::any_cast<moudmap_t>(storage);
    if (_moud_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Moud Data Transitions Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

void MoudImpl::LoadCostData(datamanagement::ModelData &model_data) {
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
        msg << "Error getting cost data in MOUD: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _cost_data = std::any_cast<costmap_t>(storage);
    if (_cost_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "MOUD Cost Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

bool MoudImpl::HistoryOfOud(const model::Person &person) const {
    return (person.GetBehaviorDetails().behavior != data::Behavior::kNever);
}

bool MoudImpl::ActiveOud(const model::Person &person) const {
    data::Behavior b = person.GetBehaviorDetails().behavior;
    return (b == data::Behavior::kInjection ||
            b == data::Behavior::kNoninjection);
}

std::vector<double>
MoudImpl::GetMoudTransitionProbability(const model::Person &person) const {
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int moud = static_cast<int>(person.GetMoudDetails().moud_state);
    int moud_duration = 0;
    if (moud == static_cast<int>(data::MOUD::kCurrent)) {
        moud_duration = static_cast<int>(
            person.GetMoudDetails().current_state_concurrent_months);
    }
    int pregnancy =
        static_cast<int>(person.GetPregnancyDetails().pregnancy_state);

    utils::tuple_4i tup =
        std::make_tuple(age_years, moud, moud_duration, pregnancy);
    std::vector<double> probs;
    try {
        probs = {_moud_data.at(tup).none, _moud_data.at(tup).current,
                 _moud_data.at(tup).post};
    } catch (std::out_of_range &e) {
        std::stringstream msg;
        msg << "MOUD Transition Probabilities are not found for the person "
               "details (age, MOUD, MOUD Duration, Pregnancy): "
            << age_years << " " << person.GetSex() << " "
            << person.GetMoudDetails().moud_state << moud_duration
            << person.GetPregnancyDetails().pregnancy_state
            << "! Returning guaranteed injection use.";
        hepce::utils::LogError(GetLogName(), msg.str());
        return {0.0, 0.0, 1.0};
    }
    return probs;
}

void MoudImpl::CalculateCostAndUtility(model::Person &person) {
    int moud_state = static_cast<int>(person.GetMoudDetails().moud_state);
    int pregnancy =
        static_cast<int>(person.GetPregnancyDetails().pregnancy_state);
    utils::tuple_2i tup = std::make_tuple(moud_state, pregnancy);

    AddEventCost(person, _cost_data[tup].cost);
    AddEventUtility(person, _cost_data[tup].util);
}
} // namespace behavior
} // namespace event
} // namespace hepce

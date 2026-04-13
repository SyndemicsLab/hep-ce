////////////////////////////////////////////////////////////////////////////////
// File: overdose.cpp                                                         //
// Project: hep-ce                                                            //
// Created Date: 2025-05-08                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/utils/logging.hpp>

#include "internals/overdose_internals.hpp"

namespace hepce {
namespace event {
// Factory
std::unique_ptr<hepce::event::Event>
Overdose::Create(const data::Inputs &inputs, const std::string &log_name) {
    return std::make_unique<Overdose>(inputs, log_name);
}

void Overdose::Execute(model::Person &person, const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // pregnancy, moud, drug_behavior, overdose_probability, cost, utility
    int pregnancy =
        static_cast<int>(person.GetPregnancyDetails().pregnancy_state);
    int moud = static_cast<int>(person.GetMoudDetails().moud_state);
    int drug_behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(pregnancy, moud, drug_behavior);
    double prob = _overdose_data[tup].overdose_probability;
    if (sampler.GetDecision({prob, 1 - prob}) == 0) {
        person.ToggleOverdose();
        CalculateCostAndUtility(person);
    }
}

void Overdose::LoadOverdoseData() {
    std::any storage = _overdose_data;
    try {
        GetInputs().SelectFromDatabase(
            OverdoseSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                utils::tuple_3i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
                    stmt.getColumn(2).getInt());
                std::any_cast<overdosemap_t &>(storage)[tup] = {
                    stmt.getColumn(3).getDouble(),
                    stmt.getColumn(4).getDouble()};
            },
            storage, {});
    } catch (std::exception &e) {
        hepce::utils::LogWarning(
            GetLogName(), "No Overdose Table Found in the inputs database...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        return;
    }

    _overdose_data = std::any_cast<overdosemap_t>(storage);
    if (_overdose_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Overdose Table is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

void Overdose::CalculateCostAndUtility(model::Person &person) {
    int pregnancy =
        static_cast<int>(person.GetPregnancyDetails().pregnancy_state);
    int moud = static_cast<int>(person.GetMoudDetails().moud_state);
    int drug_behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(pregnancy, moud, drug_behavior);

    AddEventCost(person, _overdose_data[tup].cost);
    AddEventUtility(person, _overdose_data[tup].utility);
}
} // namespace event
} // namespace hepce

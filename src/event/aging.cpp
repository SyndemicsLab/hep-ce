////////////////////////////////////////////////////////////////////////////////
// File: aging.cpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include <string>

// Library Includes
#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/math.hpp>

// Local Includes
#include "internals/aging_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> Aging::Create(const data::Inputs &inputs,
                                     const std::string &log_name) {
    return std::make_unique<Aging>(inputs, log_name);
}

// Execution
void Aging::Execute(model::Person &person, const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // Set person background utility before accumulating, which makes the first
    // timestep slightly more realistic
    AddBackgroundCostAndUtility(person);
    person.AccumulateTotalUtility(GetDiscount());
    person.Grow();
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetDiscount(), person.GetCurrentTimestep()));
}

// Private Methods
void Aging::LoadData() {
    SetCostCategory(model::CostCategory::kBackground);
    SetUtilityCategory(model::UtilityCategory::kBackground);
    _age_data.clear();
    std::any storage = agemap_t{};
    try {
        GetInputs().SelectFromDatabase(
            BuildSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                agemap_t *temp = std::any_cast<agemap_t>(&storage);
                utils::tuple_3i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
                    stmt.getColumn(2).getInt());
                data::CostUtil cu = {stmt.getColumn(3).getDouble(),
                                     stmt.getColumn(4).getDouble()};
                (*temp)[tup] = cu;
            },
            storage, {});
    } catch (std::exception &e) {
        hepce::utils::LogError(GetLogName(), e.what());
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        return;
    }
    _age_data = std::any_cast<agemap_t>(storage);
    if (_age_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Age Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

void Aging::AddBackgroundCostAndUtility(model::Person &person) const {
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, behavior);
    auto cu = _age_data.at(tup);
    AddEventCost(person, cu.cost);
    AddEventUtility(person, cu.util);
}
} // namespace event
} // namespace hepce

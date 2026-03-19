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
std::unique_ptr<Event> Aging::Create(datamanagement::ModelData &model_data,
                                     const std::string &log_name) {
    return std::make_unique<Aging>(model_data, log_name);
}

// Execution
void AgingImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // Set person background utility before accumulating, which makes the first
    // timestep slightly more realistic
    AddBackgroundCostAndUtility(person);
    person.AccumulateTotalUtility(GetEventDiscount());
    person.Grow();
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetEventDiscount(), person.GetCurrentTimestep()));
}

// Private Methods
void AgingImpl::LoadData(datamanagement::ModelData &model_data) {
    SetEventCostCategory(model::CostCategory::kBackground);
    SetEventUtilityCategory(model::UtilityCategory::kBackground);
    _age_data.clear();
    std::any storage = agemap_t{};
    try {
        model_data.GetDBSource("inputs").Select(
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
            storage);
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

void AgingImpl::AddBackgroundCostAndUtility(model::Person &person) {
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, behavior);

    AddEventCost(person, _age_data[tup].cost);
    AddEventUtility(person, _age_data[tup].util);
}
} // namespace event
} // namespace hepce

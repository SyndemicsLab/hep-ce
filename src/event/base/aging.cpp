////////////////////////////////////////////////////////////////////////////////
// File: aging.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/base/aging.hpp>

// Library Includes
#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/math.hpp>

// Local Includes
#include "internals/aging_internals.hpp"

namespace hepce {
namespace event {
namespace base {

// Factory
std::unique_ptr<hepce::event::Event>
Aging::Create(datamanagement::ModelData &model_data,
              const std::string &log_name) {
    return std::make_unique<AgingImpl>(model_data, log_name);
}

// Constructor
AgingImpl::AgingImpl(datamanagement::ModelData &model_data,
                     const std::string &log_name)
    : EventBase(model_data, log_name) {
    _age_data.clear();
    LoadData(model_data);
}

// Execution
void AgingImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (!person.IsAlive()) {
        return;
    }
    person.AccumulateTotalUtility(GetEventDiscount());
    person.Grow();
    AddBackgroundCostAndUtility(person);
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetEventDiscount(), person.GetCurrentTimestep()));
}

// Private Methods
void AgingImpl::LoadData(datamanagement::ModelData &model_data) {
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
        return;
    }
    _age_data = std::any_cast<agemap_t>(storage);
    if (_age_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Age Data is Empty...");
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
} // namespace base
} // namespace event
} // namespace hepce
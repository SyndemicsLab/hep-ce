////////////////////////////////////////////////////////////////////////////////
// File: aging.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/base/aging.hpp>

#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>
#include <hepce/utils/math.hpp>

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
int AgingImpl::Execute(model::Person &person, model::Sampler &sampler) {
    person.Grow();
    AddBackgroundCostAndUtility(person);
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetEventDiscount(), person.GetCurrentTimestep()));
}

// Private Methods
int AgingImpl::LoadData(datamanagement::ModelData &model_data) {
    std::string query = BuildSQL();
    std::string error;
    std::any storage = _age_data;
    model_data.GetDBSource("inputs").Select(
        query,
        [](std::any &storage, const SQLite::Statement &stmt) {
            utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                                  stmt.getColumn(1).getInt(),
                                                  stmt.getColumn(2).getInt());
            data::CostUtil cu = {stmt.getColumn(3).getDouble(),
                                 stmt.getColumn(4).getDouble()};
            std::any_cast<agemap_t>(storage)[tup] = cu;
        },
        storage);
    _age_data = std::any_cast<agemap_t>(storage);
    if (_age_data.empty()) {
        // Warn Empty
    }
    return 0;
}

void AgingImpl::AddBackgroundCostAndUtility(model::Person &person) {
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, behavior);

    SetEventCost(_age_data[tup].cost);
    AddEventCost(person);

    SetEventUtility(_age_data[tup].util);
    AddEventUtility(person);
}
} // namespace base
} // namespace event
} // namespace hepce
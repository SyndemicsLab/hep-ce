////////////////////////////////////////////////////////////////////////////////
// File: aging.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
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
std::unique_ptr<hepce::event::Event>
Aging::Create(datamanagement::ModelData &model_data,
              const std::string &log_name) {
    return std::make_unique<AgingImpl>(model_data, log_name);
}
int AgingImpl::Execute(model::Person &person, model::Sampler &sampler) {
    person.Grow();
    AddBackgroundCostAndUtility(person);
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetDiscount(), person.GetCurrentTimestep()));
}

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
    int behavior = static_cast<int>(person.GetBehavior());
    utils::tuple_3i tup = std::make_tuple(age_years, gender, behavior);

    SetCost(_age_data[tup].cost);
    AddEventCost(person);

    person.SetUtility(_age_data[tup].util, GetUtilityCategory());
    std::pair<double, double> utilities = person.GetUtility();
    std::pair<double, double> discount_utilities = {
        utils::Discount(utilities.first, GetDiscount(),
                        person.GetCurrentTimestep()),
        utils::Discount(utilities.second, GetDiscount(),
                        person.GetCurrentTimestep())};
    person.AccumulateTotalUtility(utilities, discount_utilities);
}
} // namespace base
} // namespace event
} // namespace hepce
////////////////////////////////////////////////////////////////////////////////
// File: aging.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/base/aging.hpp>

#include "internals/aging_internals.hpp"
#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace event {
namespace base {
std::unique_ptr<hepce::event::Event>
Aging::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
              const std::string &log_name) {
    return std::make_unique<AgingImpl>(dm, log_name);
}
int AgingImpl::Execute(model::Person &person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       model::Sampler &sampler) {
    person.Grow();
    AddBackgroundCostAndUtility(person, dm);
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetDiscount(), person.GetCurrentTimestep()));
}

int AgingImpl::LoadData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string query = BuildSQL();
    std::string error;
    int rc = dm->SelectCustomCallback(query, Callback, &_age_data, error);
    if (rc != 0) {
        spdlog::get("main")->error(
            "Error extracting Aging Data from background costs and "
            "background behaviors! Error Message: {}",
            error);
    }
    if (_age_data.empty()) {
        spdlog::get("main")->warn("No Background Cost found for Aging!");
    }
    return rc;
}

void AgingImpl::AddBackgroundCostAndUtility(
    model::Person &person,
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    int age_years = (int)(person.GetAge() / 12.0);
    int gender = ((int)person.GetSex());
    int behavior = ((int)person.GetBehavior());
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
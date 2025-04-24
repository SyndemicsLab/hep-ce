////////////////////////////////////////////////////////////////////////////////
// File: event_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENTINTERNALS_HPP_
#define HEPCE_EVENT_EVENTINTERNALS_HPP_

#include <hepce/event/event.hpp>

#include <hepce/data/types.hpp>
#include <hepce/model/cost.hpp>
#include <hepce/model/utility.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace event {
class EventBase : public virtual Event {
public:
    void SetDiscount(const double &d) { _discount = d; }

    void SetCost(const double &cost) { _cu.cost = cost; }
    void SetCostCategory(const model::CostCategory &cc) {
        _event_cost_category = cc;
    }

    void SetUtil(const double &util) { _cu.util = util; }
    void SetUtilityCategory(const model::UtilityCategory &uc) {
        _event_utility_category = uc;
    }

    double GetDiscount() const { return _discount; }

    double GetCost() const { return _cu.cost; }
    model::CostCategory GetCostCategory() const { return _event_cost_category; }

    double GetUtil() const { return _cu.util; }
    model::UtilityCategory GetUtilityCategory() const {
        return _event_utility_category;
    }

    void AddEventCost(model::Person &person, bool annual = false) const {
        double discounted_cost = utils::Discount(
            GetCost(), GetDiscount(), person.GetCurrentTimestep(), annual);
        person.AddCost(GetCost(), discounted_cost, GetCostCategory());
    }

    void AddUtility(model::Person &person) const {
        person.SetUtility(GetUtil(), GetUtilityCategory());
    }

private:
    double _discount = 0.0;
    model::UtilityCategory _event_utility_category =
        model::UtilityCategory::kBackground;
    model::CostCategory _event_cost_category = model::CostCategory::kBackground;
    data::CostUtil _cu;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENTINTERNALS_HPP_
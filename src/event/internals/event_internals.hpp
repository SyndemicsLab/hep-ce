////////////////////////////////////////////////////////////////////////////////
// File: event_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
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
    void SetDiscount(const double &d) { discount = d; }

    void SetCost(const double &cost) { cu.cost = cost; }
    void SetCostCategory(const model::CostCategory &cc) {
        eventCostCategory = cc;
    }

    void SetUtil(const double &util) { cu.util = util; }
    void SetUtilityCategory(const model::UtilityCategory &uc) {
        eventUtilityCategory = uc;
    }

    double GetDiscount() const { return discount; }

    double GetCost() const { return cu.cost; }
    model::CostCategory GetCostCategory() const { return eventCostCategory; }

    double GetUtil() const { return cu.util; }
    model::UtilityCategory GetUtilityCategory() const {
        return eventUtilityCategory;
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
    double discount = 0.0;
    model::UtilityCategory eventUtilityCategory =
        model::UtilityCategory::kBackground;
    model::CostCategory eventCostCategory = model::CostCategory::kBackground;
    data::CostUtil cu;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENTINTERNALS_HPP_
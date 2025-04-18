////////////////////////////////////////////////////////////////////////////////
// File: event_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
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

namespace hepce {
namespace event {
class EventBase : public virtual Event {
public:
    void SetDiscount(double d) { discount = d; }
    void SetCost(double cost) { cu.cost = cost; }
    void SetUtil(double util) { cu.util = util; }
    void SetUtilityCategory(model::UtilityCategory uc) {
        eventUtilityCategory = uc;
    }
    void SetCostCategory(model::CostCategory cc) { eventCostCategory = cc; }

    double GetDiscount() { return discount; }
    double GetCost() { return cu.cost; }
    double GetUtil() { return cu.util; }
    model::UtilityCategory GetUtilityCategory() { return eventUtilityCategory; }
    model::CostCategory GetCostCategory() { return eventCostCategory; }

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
////////////////////////////////////////////////////////////////////////////////
// File: event_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENTINTERNALS_HPP_
#define HEPCE_EVENT_EVENTINTERNALS_HPP_

#include <hepce/event/event.hpp>

#include <hepce/data/types.hpp>
#include <hepce/model/costing.hpp>
#include <hepce/model/utility.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace event {
class EventBase : public virtual Event {
public:
    EventBase(datamanagement::ModelData &model_data,
              const std::string &log_name = "console") {
        SetEventDiscount(
            utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
        SetEventCostCategory(model::CostCategory::kBackground);
        SetEventUtilityCategory(model::UtilityCategory::kBackground);
    }
    ~EventBase() = default;
    void SetEventDiscount(const double &d) { _discount = d; }

    void SetEventCost(const double &cost) { _cu.cost = cost; }
    void SetEventCostCategory(const model::CostCategory &cc) {
        _event_cost_category = cc;
    }

    void SetEventUtility(const double &util) { _cu.util = util; }
    void SetEventUtilityCategory(const model::UtilityCategory &uc) {
        _event_utility_category = uc;
    }

    double GetEventDiscount() const { return _discount; }

    double GetEventCost() const { return _cu.cost; }
    model::CostCategory GetEventCostCategory() const {
        return _event_cost_category;
    }

    double GetEventUtility() const { return _cu.util; }
    model::UtilityCategory GetEventUtilityCategory() const {
        return _event_utility_category;
    }

    void AddEventCost(model::Person &person, bool annual = false) const {
        double discounted_cost =
            utils::Discount(GetEventCost(), GetEventDiscount(),
                            person.GetCurrentTimestep(), annual);
        person.AddCost(GetEventCost(), discounted_cost, GetEventCostCategory());
    }

    void AddEventUtility(model::Person &person) const {
        person.SetUtility(GetEventUtility(), GetEventUtilityCategory());
        person.AccumulateTotalUtility(GetEventDiscount());
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
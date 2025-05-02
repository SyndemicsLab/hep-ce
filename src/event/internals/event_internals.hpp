////////////////////////////////////////////////////////////////////////////////
// File: event_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENTINTERNALS_HPP_
#define HEPCE_EVENT_EVENTINTERNALS_HPP_

// File Header
#include <hepce/event/event.hpp>

// Library Includes
#include <hepce/data/types.hpp>
#include <hepce/model/costing.hpp>
#include <hepce/model/utility.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace event {
class EventBase : public virtual Event {
public:
    EventBase(datamanagement::ModelData &model_data,
              const std::string &log_name = "console")
        : _log_name(log_name) {
        SetEventDiscount(
            utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
        SetEventCostCategory(model::CostCategory::kMisc);
        SetEventUtilityCategory(model::UtilityCategory::kBackground);
    }
    ~EventBase() = default;
    void SetEventDiscount(const double &d) { _discount = d; }
    void SetEventCostCategory(const model::CostCategory &cc) {
        _event_cost_category = cc;
    }
    void SetEventUtilityCategory(const model::UtilityCategory &uc) {
        _event_utility_category = uc;
    }

    double GetEventDiscount() const { return _discount; }
    model::CostCategory GetEventCostCategory() const {
        return _event_cost_category;
    }
    model::UtilityCategory GetEventUtilityCategory() const {
        return _event_utility_category;
    }

    std::string GetLogName() const { return _log_name; }

    void AddEventCost(model::Person &person, double event_cost,
                      bool annual = false) const {
        double discounted_cost =
            utils::Discount(event_cost, GetEventDiscount(),
                            person.GetCurrentTimestep(), annual);
        person.AddCost(event_cost, discounted_cost, GetEventCostCategory());
    }

    void AddEventUtility(model::Person &person, double event_utility) const {
        person.SetUtility(event_utility, GetEventUtilityCategory());
    }

    inline int GetTimeSince(const model::Person &person, int time) const {
        return person.GetCurrentTimestep() - time;
    }

private:
    const std::string _log_name;
    double _discount = 0.0;
    model::UtilityCategory _event_utility_category =
        model::UtilityCategory::kBackground;
    model::CostCategory _event_cost_category = model::CostCategory::kBackground;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENTINTERNALS_HPP_
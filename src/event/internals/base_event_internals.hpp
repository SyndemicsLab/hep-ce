////////////////////////////////////////////////////////////////////////////////
// File: event_internals.hpp                                                  //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENTINTERNALS_HPP_
#define HEPCE_EVENT_EVENTINTERNALS_HPP_

// File Header
#include <hepce/event/event.hpp>

// Library Includes
#include <hepce/data/inputs.hpp>
#include <hepce/data/types.hpp>
#include <hepce/model/costing.hpp>
#include <hepce/model/utility.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace event {
class EventBase : public virtual Event {
public:
    EventBase(const std::string &name, const data::Inputs &inputs,
              const std::string &log_name)
        : _name(name), _inputs(inputs), _log_name(log_name) {
        SetDiscount(
            utils::GetDoubleFromConfig("cost.discounting_rate", _inputs));
        SetCostCategory(model::CostCategory::kMisc);
        SetUtilityCategory(model::UtilityCategory::kBackground);
    }
    virtual ~EventBase() = default;

    // Getters
    const std::string &GetName() const { return _name; }
    const data::Inputs &GetInputs() const { return _inputs; }
    const std::string &GetLogName() const { return _log_name; }
    const double &GetDiscount() const { return _discount; }
    const model::UtilityCategory &GetUtilityCategory() const {
        return _event_utility_category;
    }
    const model::CostCategory &GetCostCategory() const {
        return _event_cost_category;
    }

    // Setters
    void SetDiscount(const double &d) { _discount = d; }
    void SetCostCategory(const model::CostCategory &cc) {
        _event_cost_category = cc;
    }
    void SetUtilityCategory(const model::UtilityCategory &uc) {
        _event_utility_category = uc;
    }

    // Common Event Utilities
    bool ValidExecute(const model::Person &person) const override {
        return person.IsAlive();
    }

    void AddEventCost(model::Person &person, const double &event_cost,
                      const bool &annual = false) const {
        double discounted_cost = utils::Discount(
            event_cost, GetDiscount(), person.GetCurrentTimestep(), annual);
        person.AddCost(event_cost, discounted_cost, GetCostCategory());
    }

    void AddEventUtility(model::Person &person, double event_utility) const {
        person.SetUtility(event_utility, GetUtilityCategory());
    }

    inline int GetTimeSince(const model::Person &person, int time) const {
        return person.GetCurrentTimestep() - time;
    }

private:
    const std::string _name;
    const data::Inputs _inputs;
    const std::string _log_name;
    double _discount = 0.0;
    model::UtilityCategory _event_utility_category =
        model::UtilityCategory::kBackground;
    model::CostCategory _event_cost_category = model::CostCategory::kBackground;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENTINTERNALS_HPP_
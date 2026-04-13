////////////////////////////////////////////////////////////////////////////////
// File: overdose_internals.hpp                                               //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_OVERDOSE_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_OVERDOSE_INTERNALS_HPP_

#include <hepce/utils/pair_hashing.hpp>

#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class Overdose : public virtual EventBase {
public:
    struct OverdoseData {
        double overdose_probability = 0.0;
        double cost = 0.0;
        double utility = 1.0;
    };

    using overdosemap_t =
        std::unordered_map<utils::tuple_3i, OverdoseData, utils::key_hash_3i,
                           utils::key_equal_3i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    Overdose(const data::Inputs &inputs, const std::string &log)
        : EventBase("overdose", inputs, log) {
        LoadOverdoseData();
        SetCostCategory(model::CostCategory::kOverdose);
        SetUtilityCategory(model::UtilityCategory::kOverdose);
    }
    ~Overdose() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<Overdose>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    overdosemap_t _overdose_data;

    inline const std::string OverdoseSQL() const {
        return "SELECT pregnancy, moud, drug_behavior, overdose_probability, "
               "cost, utility FROM overdoses;";
    }

    void LoadOverdoseData();

    void CalculateCostAndUtility(model::Person &person);
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BEHAVIOR_OVERDOSE_INTERNALS_HPP_
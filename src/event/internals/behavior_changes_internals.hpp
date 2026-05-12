////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes_internals.hpp                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_BEHAVIORCHANGES_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_BEHAVIORCHANGES_INTERNALS_HPP_

#include "base_event_internals.hpp"

#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
class BehaviorChanges : public virtual EventBase {
public:
    struct behavior_transitions {
        double never = 0.0;
        double fni = 0.25;
        double fi = 0.25;
        double ni = 0.25;
        double in = 0.25;
    };
    using behaviormap_t =
        std::unordered_map<utils::tuple_4i, struct behavior_transitions,
                           utils::key_hash_4i, utils::key_equal_4i>;

    using costmap_t =
        std::unordered_map<utils::tuple_2i, data::CostUtil, utils::key_hash_2i,
                           utils::key_equal_2i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    // Constructor
    BehaviorChanges(const data::Inputs &inputs, const std::string &log)
        : EventBase("behavior_changes", inputs, log),
          _first_year_relapse_rate(utils::GetDoubleFromConfig(
              "behavior.first_year_relapse_rate", inputs)),
          _later_years_relapse_rate(utils::GetDoubleFromConfig(
              "behavior.later_years_relapse_rate", inputs)) {
        SetCostCategory(model::CostCategory::kBehavior);
        SetUtilityCategory(model::UtilityCategory::kBehavior);
        LoadCostData();
        LoadBehaviorData();
    }

    // Destructor
    ~BehaviorChanges() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<BehaviorChanges>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    behaviormap_t _behavior_data;
    costmap_t _cost_data;
    const double _first_year_relapse_rate;
    const double _later_years_relapse_rate;

    inline const std::string TransitionSQL() const {
        return "SELECT age_years, gender, drug_behavior, moud, never, "
               "former_noninjection, former_injection, "
               "noninjection, injection FROM behavior_transitions;";
    }

    inline const std::string CostSQL() const {
        return "SELECT gender, drug_behavior, cost, utility FROM "
               "behavior_impacts;";
    }

    inline double GetExponentialChange(int time_since_quit) const {

        // Magic Number 12 corresponds to 12 months in a year
        if (time_since_quit >= 1 && time_since_quit <= 12) {
            return std::exp(-_first_year_relapse_rate *
                            (static_cast<double>(time_since_quit) - 1));
        } else if (time_since_quit > 12) {
            return std::exp(-_first_year_relapse_rate * (12 - 1)) *
                   std::exp(-_later_years_relapse_rate *
                            (time_since_quit - 12));
        }
        utils::LogWarning(GetLogName(),
                          "Time Since Quit is less than 1 month, returning 1.0 "
                          "as exponential change value.");
        return 1.0;
    }

    void
    ApplyDecayToRelapseProbabilities(std::vector<double> &probs,
                                     double decay_value,
                                     data::Behavior current_behavior) const;

    std::vector<double>
    GetBehaviorTransitionProbabilities(const model::Person &person) const;

    void LoadCostData();
    void LoadBehaviorData();

    void CalculateCostAndUtility(model::Person &person) const;
};
} // namespace event
} // namespace hepce

#endif
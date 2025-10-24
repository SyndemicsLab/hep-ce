////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes_internals.hpp                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_BEHAVIORCHANGES_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_BEHAVIORCHANGES_INTERNALS_HPP_

#include <hepce/event/behavior/behavior_changes.hpp>

#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
class BehaviorChangesImpl : public virtual BehaviorChanges, public EventBase {
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

    BehaviorChangesImpl(datamanagement::ModelData &model_data,
                        const std::string &log_name = "console");
    ~BehaviorChangesImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

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
        double relapse_rate = (time_since_quit < 12)
                                  ? _first_year_relapse_rate
                                  : _later_years_relapse_rate;

        return std::exp(-relapse_rate * static_cast<double>(time_since_quit));
    }

    void
    ApplyDecayToRelapseProbabilities(std::vector<double> &probs,
                                     double decay_value,
                                     data::Behavior current_behavior) const;

    std::vector<double>
    GetBehaviorTransitionProbabilities(const model::Person &person) const;

    void LoadCostData(datamanagement::ModelData &model_data);

    void LoadBehaviorData(datamanagement::ModelData &model_data);

    void CalculateCostAndUtility(model::Person &person);
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif
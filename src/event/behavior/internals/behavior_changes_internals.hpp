////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes_internals.hpp                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_BEHAVIORCHANGES_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_BEHAVIORCHANGES_INTERNALS_HPP_

#include <hepce/event/behavior/behavior_changes.hpp>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace behavior {
class BehaviorChangesImpl : public virtual BehaviorChanges, public EventBase {
public:
    BehaviorChangesImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                        const std::string &log_name = "console");
    ~BehaviorChangesImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
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
    behaviormap_t behavior_data;

    using costmap_t =
        std::unordered_map<utils::tuple_2i, data::CostUtil, utils::key_hash_2i,
                           utils::key_equal_2i>;
    costmap_t cost_data;

    inline const std::string TransitionSQL() const {
        return "SELECT age_years, gender, moud, drug_behavior, never, "
               "former_noninjection, former_injection, "
               "noninjection, injection FROM behavior_transitions;";
    }

    static int callback_trans(void *storage, int count, char **data,
                              char **columns) {
        utils::tuple_4i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]),
                            std::stoi(data[2]), std::stoi(data[3]));
        struct behavior_transitions behavior = {
            utils::SToDPositive(data[4]), utils::SToDPositive(data[5]),
            utils::SToDPositive(data[6]), utils::SToDPositive(data[7]),
            utils::SToDPositive(data[8])};
        (*((behaviormap_t *)storage))[tup] = behavior;
        return 0;
    }

    inline const std::string CostSQL() const {
        return "SELECT gender, drug_behavior, cost, utility FROM "
               "behavior_impacts;";
    }

    static int callback_costs(void *storage, int count, char **data,
                              char **columns) {
        utils::tuple_2i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
        data::CostUtil cu = {utils::SToDPositive(data[2]),
                             utils::SToDPositive(data[3])};
        (*((costmap_t *)storage))[tup] = cu;
        return 0;
    }

    int LoadCostData(std::shared_ptr<datamanagement::DataManagerBase> dm);

    int LoadBehaviorData(std::shared_ptr<datamanagement::DataManagerBase> dm);

    void BehaviorChangesImpl::CalculateCostAndUtility(model::Person &person);
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif
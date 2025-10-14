////////////////////////////////////////////////////////////////////////////////
// File: moud_internals.hpp                                                   //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_MOUD_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_MOUD_INTERNALS_HPP_

#include <hepce/event/behavior/moud.hpp>

#include <hepce/utils/pair_hashing.hpp>

#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
class MoudImpl : public virtual Moud, public EventBase {
public:
    struct moud_transitions {
        double none = 0.33;
        double current = 0.34;
        double post = 0.33;
    };
    using moudmap_t =
        std::unordered_map<utils::tuple_4i, struct moud_transitions,
                           utils::key_hash_4i, utils::key_equal_4i>;

    using costmap_t =
        std::unordered_map<utils::tuple_2i, data::CostUtil, utils::key_hash_2i,
                           utils::key_equal_2i>;

    MoudImpl(datamanagement::ModelData &model_data,
             const std::string &log_name = "console");
    ~MoudImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    moudmap_t _moud_data;
    costmap_t _cost_data;

    inline const std::string TransitionSQL() const {
        return "SELECT age_years, current_moud, current_duration, pregnancy, "
               "none, "
               "current, post FROM moud_transitions;";
    }

    inline const std::string CostSQL() const {
        return "SELECT moud, pregnancy, cost, utility FROM moud_costs;";
    }

    void LoadCostData(datamanagement::ModelData &model_data);
    void LoadMOUDData(datamanagement::ModelData &model_data);

    bool HistoryOfOud(const model::Person &person) const;
    bool ActiveOud(const model::Person &person) const;
    std::vector<double>
    GetMoudTransitionProbability(const model::Person &person) const;
    void CalculateCostAndUtility(model::Person &person);
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BEHAVIOR_MOUD_INTERNALS_HPP_
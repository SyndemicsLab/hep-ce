////////////////////////////////////////////////////////////////////////////////
// File: overdose_internals.hpp                                               //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_OVERDOSE_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_OVERDOSE_INTERNALS_HPP_

#include <hepce/event/behavior/overdose.hpp>

#include <hepce/utils/pair_hashing.hpp>

#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
class OverdoseImpl : public virtual Overdose, public EventBase {
public:
    struct OverdoseData {
        double overdose_probability = 0.0;
        double cost = 0.0;
        double utility = 1.0;
    };

    using overdosemap_t =
        std::unordered_map<utils::tuple_3i, OverdoseData, utils::key_hash_3i,
                           utils::key_equal_3i>;

    OverdoseImpl(datamanagement::ModelData &model_data,
                 const std::string &log_name = "console");
    ~OverdoseImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    overdosemap_t _overdose_data;

    inline const std::string OverdoseSQL() const {
        return "SELECT pregnancy, moud, drug_behavior, overdose_probability, "
               "cost, utility FROM overdoses;";
    }

    void LoadOverdoseData(datamanagement::ModelData &model_data);

    void CalculateCostAndUtility(model::Person &person);
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BEHAVIOR_OVERDOSE_INTERNALS_HPP_
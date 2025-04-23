////////////////////////////////////////////////////////////////////////////////
// File: progression_internals.hpp                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_FIBROSIS_PROGRESSIONINTERNALS_HPP_
#define HEPCE_EVENT_FIBROSIS_PROGRESSIONINTERNALS_HPP_

#include <hepce/event/fibrosis/progression.hpp>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace fibrosis {
class ProgressionImpl : public virtual Progression, public EventBase {
public:
    ProgressionImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                    const std::string &log_name = "console");
    ~ProgressionImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    const std::vector<double>
    GetTransitionProbability(const data::FibrosisState &fs) const;

    bool add_cost_data = false;
    struct progression_probabilities {
        double f0_to_1 = 0.0;
        double f1_to_2 = 0.0;
        double f2_to_3 = 0.0;
        double f3_to_4 = 0.0;
        double f4_to_d = 0.0;
    };

    progression_probabilities probabilities;
    using costutilmap_t =
        std::unordered_map<utils::tuple_2i, data::CostUtil, utils::key_hash_2i,
                           utils::key_equal_2i>;
    costutilmap_t cost_data;

    inline const std::string ProgressionSQL() const {
        return "SELECT hcv_status, fibrosis_state, cost, utility FROM "
               "hcv_impacts;";
    }

    static int Callback(void *storage, int count, char **data, char **columns) {
        utils::tuple_2i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
        (*((costutilmap_t *)storage))[tup] = {utils::SToDPositive(data[2]),
                                              utils::SToDPositive(data[3])};
        return 0;
    }

    inline const utils::tuple_2i
    TupleBuilder(const model::Person &person) const {
        int hcv_status = (person.GetHCV() == data::HCV::NONE) ? 0 : 1;
        int fibrosis_state = static_cast<int>(person.GetTrueFibrosisState());
        return std::make_tuple(hcv_status, fibrosis_state);
    }

    inline void AddProgressionCost(model::Person &person) {
        SetCost(cost_data[TupleBuilder(person)].cost);
        AddEventCost(person);
    }

    inline void AddProgressionUtility(model::Person &person) {
        SetUtil(cost_data[TupleBuilder(person)].util);
        AddUtility(person);
    }

    void
    GetProgressionData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(ProgressionSQL(), Callback,
                                          &cost_data, error);
        if (rc != 0) {
            // spdlog::get("main")->error(
            //     "No cost and/or utility avaliable for Fibrosis "
            //     "Progression! Error Message: {}",
            //     error);
        }
        if (cost_data.empty()) {
            // spdlog::get("main")->warn(
            //     "No cost and/or utility found for Fibrosis Progression.");
        }
    }
};
} // namespace fibrosis
} // namespace event
} // namespace hepce

#endif
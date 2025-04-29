////////////////////////////////////////////////////////////////////////////////
// File: progression_internals.hpp                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
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
    struct progression_probabilities {
        double f0_to_1 = 0.0;
        double f1_to_2 = 0.0;
        double f2_to_3 = 0.0;
        double f3_to_4 = 0.0;
        double f4_to_d = 0.0;
    };

    using costutilmap_t =
        std::unordered_map<utils::tuple_2i, data::CostUtil, utils::key_hash_2i,
                           utils::key_equal_2i>;

    ProgressionImpl(datamanagement::ModelData &model_data,
                    const std::string &log_name = "console");
    ~ProgressionImpl() = default;

    int Execute(model::Person &person, model::Sampler &sampler) override;

private:
    bool _add_cost_data = false;
    progression_probabilities _probabilities;
    costutilmap_t _cost_data;

    const std::vector<double>
    GetTransitionProbability(const data::FibrosisState &fs) const;

    inline const std::string ProgressionSQL() const {
        return "SELECT hcv_status, fibrosis_state, cost, utility FROM "
               "hcv_impacts;";
    }

    inline const utils::tuple_2i
    TupleBuilder(const model::Person &person) const {
        int hcv_status =
            (person.GetHCVDetails().hcv == data::HCV::kNone) ? 0 : 1;
        int fibrosis_state =
            static_cast<int>(person.GetHCVDetails().fibrosis_state);
        return std::make_tuple(hcv_status, fibrosis_state);
    }

    inline void AddProgressionCost(model::Person &person) {
        SetEventCost(_cost_data[TupleBuilder(person)].cost);
        AddEventCost(person);
    }

    inline void AddProgressionUtility(model::Person &person) {
        SetEventUtility(_cost_data[TupleBuilder(person)].util);
        AddEventUtility(person);
    }

    void GetProgressionData(datamanagement::ModelData &model_data) {
        std::any storage = _cost_data;
        model_data.GetDBSource("inputs").Select(
            ProgressionSQL(),
            [](std::any &storage, const SQLite::Statement &stmt) {
                utils::tuple_2i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt());
                std::any_cast<costutilmap_t>(storage)[tup] = {
                    stmt.getColumn(2).getDouble(),
                    stmt.getColumn(3).getDouble()};
            },
            storage);
        _cost_data = std::any_cast<costutilmap_t>(storage);
        if (_cost_data.empty()) {
            // Warn No Data
        }
    }
};
} // namespace fibrosis
} // namespace event
} // namespace hepce

#endif
////////////////////////////////////////////////////////////////////////////////
// File: staging_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_FIBROSIS_STAGINGINTERNALS_HPP_
#define HEPCE_EVENT_FIBROSIS_STAGINGINTERNALS_HPP_

#include <hepce/event/fibrosis/staging.hpp>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace fibrosis {
class StagingImpl : public virtual Staging, public EventBase {
public:
    using testmap_t =
        std::unordered_map<utils::tuple_2i, double, utils::key_hash_2i,
                           utils::key_equal_2i>;

    StagingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console");
    ~StagingImpl() = default;

    int Execute(model::Person &person, model::Sampler &sampler) override;

private:
    const std::string _test_one;
    const std::string _test_two;
    int _staging_period;
    double _test_one_cost;
    double _test_two_cost;
    std::string _multitest_result_method;
    std::vector<data::FibrosisState> _testtwo_eligible_fibs;
    testmap_t _test1_data;
    testmap_t _test2_data;

    static void Callback(std::any &storage, const SQLite::Statement &stmt) {
        utils::tuple_2i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt());
        std::any_cast<testmap_t>(storage)[tup] = stmt.getColumn(2).getDouble();
    }

    inline const std::string StagingSQL(const std::string &column) const {
        return "SELECT fibrosis_state, diagnosed_fibrosis, " + column +
               " FROM fibrosis;";
    }

    const std::vector<double> ProbabilityBuilder(const model::Person &person,
                                                 const testmap_t &test) const;

    void AddStagingCost(model::Person &person, const double &cost) {
        SetCost(cost);
        AddEventCost(person);
    }

    int LoadStagingData(datamanagement::ModelData &model_data);
    void LoadTestOneStagingData(datamanagement::ModelData &model_data);
    void LoadTestTwoStagingData(datamanagement::ModelData &model_data);
};
} // namespace fibrosis
} // namespace event
} // namespace hepce

#endif
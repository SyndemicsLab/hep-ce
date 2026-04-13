////////////////////////////////////////////////////////////////////////////////
// File: staging_internals.hpp                                                //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_FIBROSIS_STAGINGINTERNALS_HPP_
#define HEPCE_EVENT_FIBROSIS_STAGINGINTERNALS_HPP_

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class Staging : public virtual EventBase {
public:
    using testmap_t =
        std::unordered_map<utils::tuple_2i, double, utils::key_hash_2i,
                           utils::key_equal_2i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    Staging(const data::Inputs &inputs, const std::string &log)
        : EventBase("staging", inputs, log),
          _test_one(
              utils::GetStringFromConfig("fibrosis_staging.test_one", inputs)),
          _test_two(
              utils::GetStringFromConfig("fibrosis_staging.test_two", inputs)) {
        LoadData();
    }

    ~Staging() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<Staging>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

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

    void LoadData();

    static void Callback(std::any &storage, const SQLite::Statement &stmt) {
        testmap_t *temp = std::any_cast<testmap_t>(&storage);
        utils::tuple_2i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt());
        (*temp)[tup] = stmt.getColumn(2).getDouble();
    }

    inline const std::string StagingSQL(const std::string &column) const {
        return "SELECT fibrosis_state, diagnosed_fibrosis, " + column +
               " FROM fibrosis;";
    }

    const std::vector<double> ProbabilityBuilder(const model::Person &person,
                                                 const testmap_t &test) const;

    inline void AddStagingCost(model::Person &person, const double &cost) {
        AddEventCost(person, cost);
    }

    void LoadTestOneStagingData();
    void LoadTestTwoStagingData();
};
} // namespace event
} // namespace hepce

#endif
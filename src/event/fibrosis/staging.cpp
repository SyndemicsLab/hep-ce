////////////////////////////////////////////////////////////////////////////////
// File: staging.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/fibrosis/staging.hpp>

#include "internals/staging_internals.hpp"
#include <hepce/utils/config.hpp>
#include <hepce/utils/logging.hpp>

namespace hepce {
namespace event {
namespace fibrosis {

// Factory
std::unique_ptr<hepce::event::Event>
Staging::Create(datamanagement::ModelData &model_data,
                const std::string &log_name) {
    return std::make_unique<StagingImpl>(model_data, log_name);
}

// Constructor
StagingImpl::StagingImpl(datamanagement::ModelData &model_data,
                         const std::string &log_name)
    : EventBase(model_data, log_name),
      _test_one(
          utils::GetStringFromConfig("fibrosis_staging.test_one", model_data)),
      _test_two(
          utils::GetStringFromConfig("fibrosis_staging.test_two", model_data)) {
    LoadData(model_data);
}

// Execute
void StagingImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // 0. Check if Person even has Fibrosis, exit if they are none
    if (person.GetHCVDetails().fibrosis_state == data::FibrosisState::kNone) {
        return;
    }

    int time = person.GetFibrosisStagingDetails().time_of_last_staging;

    // 1. Check the time since the person's last fibrosis staging test.
    // If the person's last test is more recent than the limit, exit
    // event. If they've never been staged they have a -1
    if (GetTimeSince(person, time) < _staging_period && time != -1) {
        return;
    }

    // 2. Check the person's true fibrosis state and use it to search
    // for the input table to grab only test characteristics for this
    // state.

    // 3. Get the probability of each of the test_one fibrosis outcomes.
    std::vector<double> probs = ProbabilityBuilder(person, _test1_data);

    // 4. Decide which stage is assigned to the person
    int res = sampler.GetDecision(probs);
    if (res >= static_cast<int>(data::MeasuredFibrosisState::kCount)) {
        spdlog::get("main")->error("Measured Fibrosis State Decision returned "
                                   "value outside bounds");
        return;
    }
    data::MeasuredFibrosisState stateOne =
        static_cast<data::MeasuredFibrosisState>(res);

    // 5. Assign this value as the person's measured state.
    person.DiagnoseFibrosis(stateOne);
    AddStagingCost(person, _test_one_cost);

    // 6. Get a vector of the probabilities of each of the possible
    // fibrosis outcomes (test two) provided there is a second test.
    // If No Second Test Specified or not an eligible fibrosis state,
    // End
    if (_test_two.empty() ||
        !utils::FindInVector<data::FibrosisState>(
            _testtwo_eligible_fibs, {person.GetHCVDetails().fibrosis_state})) {
        return;
    }
    probs = ProbabilityBuilder(person, _test2_data);

    // 7. Decide which stage is assigned to the person.
    if (probs.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Unable to get fibrosis staging test two "
                                 "probabilities. Returning...");
        return;
    }

    person.GiveSecondStagingTest();

    data::MeasuredFibrosisState stateTwo =
        static_cast<data::MeasuredFibrosisState>(sampler.GetDecision(probs));

    // determine whether to use latest test value or greatest
    data::MeasuredFibrosisState measured;
    if (_multitest_result_method == "latest") {
        measured = stateTwo;
    } else if (_multitest_result_method == "maximum") {
        measured = std::max<data::MeasuredFibrosisState>(stateOne, stateTwo);
    } else {
        hepce::utils::LogWarning(GetLogName(),
                                 "Invalid multitest result provided: " +
                                     _multitest_result_method);
        return;
    }
    // 8. Assign this state to the person.
    person.DiagnoseFibrosis(measured);
    AddStagingCost(person, _test_two_cost);
}

void StagingImpl::LoadData(datamanagement::ModelData &model_data) {
    SetEventCostCategory(model::CostCategory::kStaging);

    _staging_period =
        utils::GetIntFromConfig("fibrosis_staging.period", model_data);
    _test_one_cost = utils::GetDoubleFromConfig(
        "fibrosis_staging.test_one_cost", model_data);
    _test_two_cost = utils::GetDoubleFromConfig(
        "fibrosis_staging.test_two_cost", model_data);
    _testtwo_eligible_fibs = utils::SplitToVecT<data::FibrosisState>(
        utils::GetStringFromConfig("fibrosis_staging.test_two_eligible_stages",
                                   model_data),
        ',');
    _multitest_result_method = utils::GetStringFromConfig(
        "fibrosis_staging.multitest_result_method", model_data);

    LoadTestOneStagingData(model_data);
    if (!_test_two.empty()) {
        LoadTestTwoStagingData(model_data);
    }
}

// Private Methods
const std::vector<double>
StagingImpl::ProbabilityBuilder(const model::Person &person,
                                const testmap_t &test) const {
    int fibrosis_state =
        static_cast<int>(person.GetHCVDetails().fibrosis_state);

    // Returning the probability tuples for each diagnosis of a true fibrosis state
    std::vector<utils::tuple_2i> tuples = {
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::kF01)),
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::kF23)),
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::kF4)),
        std::make_tuple(
            fibrosis_state,
            static_cast<int>(data::MeasuredFibrosisState::kDecomp))};

    std::vector<double> probs;
    for (const auto &tup : tuples) {
        probs.push_back(test.at(tup));
    }
    return probs;
}
void StagingImpl::LoadTestOneStagingData(
    datamanagement::ModelData &model_data) {
    std::any storage = testmap_t{};
    try {
        model_data.GetDBSource("inputs").Select(StagingSQL(_test_one), Callback,
                                                storage);
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting Test One Fibrosis Staging Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _test1_data = std::any_cast<testmap_t>(storage);
}
void StagingImpl::LoadTestTwoStagingData(
    datamanagement::ModelData &model_data) {
    std::any storage = testmap_t{};
    try {
        model_data.GetDBSource("inputs").Select(StagingSQL(_test_two), Callback,
                                                storage);
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting Test Two Fibrosis Staging Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _test2_data = std::any_cast<testmap_t>(storage);
}

} // namespace fibrosis
} // namespace event
} // namespace hepce
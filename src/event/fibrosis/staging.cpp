////////////////////////////////////////////////////////////////////////////////
// File: staging.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/fibrosis/staging.hpp>

#include "internals/staging_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace fibrosis {
std::unique_ptr<hepce::event::Event>
Staging::Create(datamanagement::ModelData &model_data,
                const std::string &log_name) {
    return std::make_unique<StagingImpl>(model_data, log_name);
}

StagingImpl::StagingImpl(datamanagement::ModelData &model_data,
                         const std::string &log_name)
    : _test_one(
          utils::GetStringFromConfig("fibrosis_staging.test_one", model_data)),
      _test_two(
          utils::GetStringFromConfig("fibrosis_staging.test_two", model_data)) {

    SetDiscount(
        utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
    SetCostCategory(model::CostCategory::kStaging);
    LoadStagingData(model_data);

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
}

int StagingImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // 0. Check if Person even has Fibrosis, exit if they are none
    if (person.GetTrueFibrosisState() == data::FibrosisState::NONE) {
        return;
    }

    // 1. Check the time since the person's last fibrosis staging test.
    // If the person's last test is more recent than the limit, exit
    // event. If they've never been staged they have a -1
    if (person.GetTimeSinceFibrosisStaging() < _staging_period &&
        person.GetTimeOfFibrosisStaging() != -1) {
        return;
    }

    // 2. Check the person's true fibrosis state and use it to search
    // for the input table to grab only test characteristics for this
    // state.

    // 3. Get the probability of each of the test_one fibrosis outcomes.
    std::vector<double> probs = ProbabilityBuilder(person, _test1_data);

    // 4. Decide which stage is assigned to the person
    int res = sampler.GetDecision(probs);
    if (res >= static_cast<int>(data::MeasuredFibrosisState::COUNT)) {
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
        std::find(_testtwo_eligible_fibs.begin(), _testtwo_eligible_fibs.end(),
                  person.GetTrueFibrosisState()) ==
            _testtwo_eligible_fibs.end()) {
        return;
    }
    probs = ProbabilityBuilder(person, _test2_data);

    // 7. Decide which stage is assigned to the person.
    if (probs.empty()) {
        return;
    }

    person.GiveSecondScreeningTest(true);

    data::MeasuredFibrosisState stateTwo =
        static_cast<data::MeasuredFibrosisState>(sampler.GetDecision(probs));

    // determine whether to use latest test value or greatest
    data::MeasuredFibrosisState measured;
    if (_multitest_result_method == "latest") {
        measured = stateTwo;
    } else if (_multitest_result_method == "maximum") {
        measured = std::max<data::MeasuredFibrosisState>(stateOne, stateTwo);
    } else {
        // log an error
        return;
    }
    // 8. Assign this state to the person.
    person.DiagnoseFibrosis(measured);
    AddStagingCost(person, _test_two_cost);
}

const std::vector<double>
StagingImpl::ProbabilityBuilder(const model::Person &person,
                                const testmap_t &test) const {
    int fibrosis_state = static_cast<int>(person.GetTrueFibrosisState());

    // Returning the probability tuples for each diagnosis of a true fibrosis state
    std::vector<utils::tuple_2i> tuples = {
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::F01)),
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::F23)),
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::F4)),
        std::make_tuple(fibrosis_state,
                        static_cast<int>(data::MeasuredFibrosisState::DECOMP))};

    std::vector<double> probs;
    for (const auto &tup : tuples) {
        probs.push_back(test.at(tup));
    }
    return probs;
}
void StagingImpl::LoadTestOneStagingData(
    datamanagement::ModelData &model_data) {
    std::any storage = _test1_data;
    model_data.GetDBSource("inputs").Select(StagingSQL(_test_one), Callback,
                                            storage);
    _test1_data = std::any_cast<testmap_t>(storage);
}
void StagingImpl::LoadTestTwoStagingData(
    datamanagement::ModelData &model_data) {
    std::any storage = _test2_data;
    model_data.GetDBSource("inputs").Select(StagingSQL(_test_two), Callback,
                                            storage);
    _test2_data = std::any_cast<testmap_t>(storage);
}

int StagingImpl::LoadStagingData(datamanagement::ModelData &model_data) {
    LoadTestOneStagingData(model_data);
    LoadTestTwoStagingData(model_data);
    return 0;
}

} // namespace fibrosis
} // namespace event
} // namespace hepce
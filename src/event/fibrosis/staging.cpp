////////////////////////////////////////////////////////////////////////////////
// File: staging.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
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
Staging::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                const std::string &log_name) {
    return std::make_unique<StagingImpl>(dm, log_name);
}

StagingImpl::StagingImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                         const std::string &log_name)
    : _test_one(utils::GetStringFromConfig("fibrosis_staging.test_one", dm)),
      _test_two(utils::GetStringFromConfig("fibrosis_staging.test_two", dm)) {

    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    SetCostCategory(model::CostCategory::kStaging);
    LoadStagingData(dm);

    _staging_period = utils::GetIntFromConfig("fibrosis_staging.period", dm);
    _test_one_cost =
        utils::GetDoubleFromConfig("fibrosis_staging.test_one_cost", dm);
    _test_two_cost =
        utils::GetDoubleFromConfig("fibrosis_staging.test_two_cost", dm);
    _testtwo_eligible_fibs = utils::SplitToVecT<data::FibrosisState>(
        utils::GetStringFromConfig("fibrosis_staging.test_two_eligible_stages",
                                   dm),
        ',');
    _multitest_result_method = utils::GetStringFromConfig(
        "fibrosis_staging.multitest_result_method", dm);
}

int StagingImpl::Execute(model::Person &person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         model::Sampler &sampler) {
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

int StagingImpl::LoadStagingData(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string error;
    int rc = dm->SelectCustomCallback(StagingSQL(_test_one), Callback,
                                      &_test1_data, error);
    if (rc != 0) {
        // Logging error
    }

    rc = dm->SelectCustomCallback(StagingSQL(_test_two), Callback, &_test2_data,
                                  error);
    return rc;
}

} // namespace fibrosis
} // namespace event
} // namespace hepce
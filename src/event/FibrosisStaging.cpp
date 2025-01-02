//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the Fibrosis Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "FibrosisStaging.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class FibrosisStaging::FibrosisStagingIMPL {
    private:
        double discount = 0.0;
        double test_one_cost;
        double test_two_cost;
        int staging_period;
        std::string test_one;
        std::unordered_map<int, std::vector<double>> test_one_data;
        std::string test_two;
        std::unordered_map<int, std::vector<double>> test_two_data;
        std::string multitest_result_method;
        std::vector<person::FibrosisState> testtwo_eligible_fibs;

        static int callback_fibrosis_test(void *storage, int count, char **data,
                                          char **columns) {
            int key = std::stoi(data[0]);
            if ((*((std::unordered_map<int, std::vector<double>> *)storage))
                    .find(key) ==
                (*((std::unordered_map<int, std::vector<double>> *)storage))
                    .end()) {
                (*((std::unordered_map<int, std::vector<double>> *)
                       storage))[key] = {};
            }
            double i = std::stod(data[1]);
            (*((std::unordered_map<int, std::vector<double>> *)storage))[key]
                .push_back(i);
            return 0;
        }

        std::string FibrosisTestSQL(std::string column) {
            return "SELECT fibrosis_state, " + column + " FROM fibrosis;";
        }

        /// @brief Aggregate the fibrosis stage testing probabilities for a
        /// given Person object.
        /// @details Fibrosis staging inputs are provided both in the tabular
        /// inputs and in the text-based configuration:
        // clang-format off
        /// | Configuration Input Name | Description |
        /// | ------------------------ | ----------- |
        /// | period                   | Number of months between staging tests.                                                                                                              |
        /// | multitest_result_method  | (Optional) Possible values: latest, maximum. Specifies whether to use the latest test result or the maximum test result when using two test methods. |
        /// | test_one                 | Name of fibrosis staging test one. Must match a column header in the fibrosis staging tabular input.                                                 |
        /// | test_one_cost            | Cost of fibrosis staging test one in USD.                                                                                                            |
        /// | test_two                 | (Optional) Name of fibrosis staging test two. Must match a column header in the fibrosis staging tabular input.                                      |
        /// | test_two_cost            | Cost of fibrosis staging test two in USD.                                                                                                            |
        /// | test_two_eligible_stages | A comma-separated list of which outcomes from test one will be tested again with fibrosis staging test two.                                          |
        // clang-format on
        /// @param table
        /// @param configLookupKey
        /// @return Vector of fibrosis staging outcome probabilities
        std::vector<double> GetMeasurementProbabilities(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            std::string column) {
            int fibrosis_state = (int)person->GetTrueFibrosisState();
            if (column == test_one) {
                return test_one_data[fibrosis_state];
            } else if (column == test_two) {
                return test_two_data[fibrosis_state];
            }
            return {};
        }

        void addStagingCost(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            const bool testTwo = false) {

            double cost = testTwo ? test_one_cost : test_two_cost;

            double discountAdjustedCost = Event::DiscountEventCost(
                cost, discount, person->GetCurrentTimestep());
            person->AddCost(cost, discountAdjustedCost,
                            cost::CostCategory::STAGING);
        }

        int
        LoadTestOneData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(FibrosisTestSQL(test_one),
                                              this->callback_fibrosis_test,
                                              &test_one_data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Fibrosis Data from fibrosis table! Error "
                    "Message: {}\nQuery: {}",
                    error, FibrosisTestSQL(test_one));
            }
            return rc;
        }

        int
        LoadTestTwoData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(FibrosisTestSQL(test_two),
                                              this->callback_fibrosis_test,
                                              &test_two_data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Fibrosis Data from fibrosis table! Error "
                    "Message: {}\nQuery: {}",
                    error, FibrosisTestSQL(test_two));
            }
            return rc;
        }

    public:
        void DoEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {

            // 0. Check if Person even has Fibrosis, exit if they are none
            if (person->GetTrueFibrosisState() == person::FibrosisState::NONE) {
                return;
            }

            // 1. Check the time since the person's last fibrosis staging test.
            // If the person's last test is more recent than the limit, exit
            // event. If they've never been staged they have a -1
            if (person->GetTimeSinceFibrosisStaging() < staging_period &&
                person->GetTimeOfFibrosisStaging() != -1) {
                return;
            }

            // 2. Check the person's true fibrosis state and use it to search
            // for the input table to grab only test characteristics for this
            // state.

            // 3. Get the probability of each of the test_one fibrosis outcomes.
            std::vector<double> probs =
                GetMeasurementProbabilities(person, dm, test_one);

            // 4. Decide which stage is assigned to the person
            int res = decider->GetDecision(probs);
            if (res >= (int)person::MeasuredFibrosisState::COUNT) {
                spdlog::get("main")->error(
                    "Measured Fibrosis State Decision returned "
                    "value outside bounds");
                return;
            }
            person::MeasuredFibrosisState stateOne =
                (person::MeasuredFibrosisState)res;

            // 5. Assign this value as the person's measured state.
            person->DiagnoseFibrosis(stateOne);
            this->addStagingCost(person, dm);

            // 6. Get a vector of the probabilities of each of the possible
            // fibrosis outcomes (test two) provided there is a second test.
            // If No Second Test Specified or not an eligible fibrosis state,
            // End
            if (test_two.empty() || std::find(testtwo_eligible_fibs.begin(),
                                              testtwo_eligible_fibs.end(),
                                              person->GetTrueFibrosisState()) ==
                                        testtwo_eligible_fibs.end()) {
                return;
            }
            probs = GetMeasurementProbabilities(person, dm, test_two);

            // 7. Decide which stage is assigned to the person->
            if (probs.empty()) {
                return;
            }

            person->GiveSecondScreeningTest(true);

            person::MeasuredFibrosisState stateTwo =
                (person::MeasuredFibrosisState)(decider->GetDecision(probs));

            // determine whether to use latest test value or greatest
            person::MeasuredFibrosisState measured;
            if (multitest_result_method == "latest") {
                measured = stateTwo;
            } else if (multitest_result_method == "maximum") {
                measured =
                    std::max<person::MeasuredFibrosisState>(stateOne, stateTwo);
            } else {
                // log an error
                return;
            }
            // 8. Assign this state to the person->
            person->DiagnoseFibrosis(measured);
            this->addStagingCost(person, dm, true);
        }
        FibrosisStagingIMPL(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string discount_data;
            int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
            if (!discount_data.empty()) {
                this->discount = std::stod(discount_data);
            }
            std::string data;

            dm->GetFromConfig("fibrosis_staging.test_one_cost", data);
            test_one_cost = (!data.empty()) ? std::stod(data) : 0.0;

            dm->GetFromConfig("fibrosis_staging.test_two_cost", data);
            test_two_cost = (!data.empty()) ? std::stod(data) : 0.0;

            dm->GetFromConfig("fibrosis_staging.period", data);
            staging_period = std::stoi(data);

            dm->GetFromConfig("fibrosis_staging.test_one", data);
            test_one = data;

            LoadTestOneData(dm);

            dm->GetFromConfig("fibrosis_staging.test_two", data);
            test_two = data;

            if (!test_two.empty()) {
                LoadTestTwoData(dm);
            }

            dm->GetFromConfig("fibrosis_staging.multitest_result_method", data);
            multitest_result_method = data;

            dm->GetFromConfig("fibrosis_stating.test_two_eligible_stages",
                              data);
            testtwo_eligible_fibs =
                Utils::split2vecT<person::FibrosisState>(data, ',');
        }
    };

    FibrosisStaging::FibrosisStaging(
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<FibrosisStagingIMPL>(dm);
    }

    FibrosisStaging::~FibrosisStaging() = default;
    FibrosisStaging::FibrosisStaging(FibrosisStaging &&) noexcept = default;
    FibrosisStaging &
    FibrosisStaging::operator=(FibrosisStaging &&) noexcept = default;

    void FibrosisStaging::DoEvent(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
        std::shared_ptr<stats::DeciderBase> decider) {
        impl->DoEvent(person, dm, decider);
    }

} // namespace event

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
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class FibrosisStaging::FibrosisStagingIMPL {
    private:
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            d->push_back(std::stod(data[0]));
            return 0;
        }
        std::string buildSQL(std::shared_ptr<person::PersonBase> person,
                             std::string columns) {
            std::stringstream sql;
            sql << "SELECT " << columns << " FROM fibrosis ";
            sql << "WHERE true_fib = '" << person->GetTrueFibrosisState()
                << "'";
            sql << " LIMIT 4;"; // weird issue with returning a bunch hits
            return sql.str();
        }

        std::string getColumnNameFromConfig(
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            std::string configLookupKey) {
            std::string column;
            column.clear();
            dm->GetFromConfig(configLookupKey, column);
            return column;
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
        std::vector<double> getTransitionProbabilities(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            std::string column) {
            std::string query = buildSQL(person, column);
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Fibrosis Data from fibrosis table! Error "
                    "Message: {}",
                    error);
                spdlog::get("main")->info("Query: {}", query);
                return {};
            }

            return storage;
        }

        void addStagingCost(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            const bool testTwo = false) {
            std::string data;
            data.clear();
            dm->GetFromConfig("fibrosis_staging.test_one_cost", data);
            double t1 = (!data.empty()) ? std::stod(data) : 0.0;

            data.clear();
            dm->GetFromConfig("fibrosis_staging.test_two_cost", data);
            double t2 = (!data.empty()) ? std::stod(data) : 0.0;

            double cost = testTwo ? t1 : t2;

            std::string costName = "Fibrosis Staging Cost (Test" +
                                   ((std::string)(testTwo ? "1" : "2")) + ")";
            cost::Cost fibrosisStagingCost = {cost::CostCategory::STAGING,
                                              costName, cost};
            person->AddCost(fibrosisStagingCost);
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {

            // 0. Check if Person even has Fibrosis, exit if they are none
            if (person->GetTrueFibrosisState() == person::FibrosisState::NONE) {
                return;
            }

            // 1. Check the time since the person's last fibrosis staging test.
            // If the person's last test is more recent than the limit, exit
            // event. If they've never been staged they have a -1
            std::string data;
            dm->GetFromConfig("fibrosis_staging.period", data);
            int period = std::stoi(data);
            if (person->GetTimeSinceFibrosisStaging() < period &&
                person->GetTimeOfFibrosisStaging() != -1) {
                return;
            }

            // 2. Check the person's true fibrosis state and use it to search
            // for the input table to grab only test characteristics for this
            // state.

            // 3. Get a vector of the probabilities of each of the possible
            // fibrosis outcomes (test one).
            data.clear();
            dm->GetFromConfig("fibrosis_staging.test_one", data);
            std::vector<double> probs =
                getTransitionProbabilities(person, dm, data);

            // 4. Decide which stage is assigned to the person->
            int res = decider->GetDecision(probs) + 1;
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
            data.clear();
            dm->GetFromConfig("fibrosis_staging.test_two", data);

            // If No Second Test Specified, End
            if (data.empty()) {
                return;
            }
            probs = getTransitionProbabilities(person, dm, data);

            // 7. Decide which stage is assigned to the person->
            if (probs.empty()) {
                return;
            }

            person->GiveSecondScreeningTest(true);

            person::MeasuredFibrosisState stateTwo =
                (person::MeasuredFibrosisState)(decider->GetDecision(probs) +
                                                1);

            // determine whether to use latest test value or greatest
            data.clear();
            dm->GetFromConfig("fibrosis_staging.multitest_result_method", data);

            person::MeasuredFibrosisState measured;
            if (data == "latest") {
                measured = stateTwo;
            } else if (data == "maximum") {
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
    };

    FibrosisStaging::FibrosisStaging() {
        impl = std::make_unique<FibrosisStagingIMPL>();
    }

    FibrosisStaging::~FibrosisStaging() = default;
    FibrosisStaging::FibrosisStaging(FibrosisStaging &&) noexcept = default;
    FibrosisStaging &
    FibrosisStaging::operator=(FibrosisStaging &&) noexcept = default;

    void FibrosisStaging::doEvent(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
        std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event

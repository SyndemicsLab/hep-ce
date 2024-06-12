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

namespace Event {
    void FibrosisStaging::doEvent(std::shared_ptr<Person::Person> person) {
        // 1. Check the time since the person's last fibrosis staging test. If
        // the person's last test is more recent than the limit, exit event.
        int timeSinceStaging =
            this->getCurrentTimestep() - person->getTimeOfLastStaging();
        if (timeSinceStaging < 0) {
            // log an error
            return;
        }

        int period =
            std::get<int>(this->config.get("fibrosis_staging.period", (int)-1));
        if (period <= 0) {
            // log error
        }
        if (((int)timeSinceStaging) < period) {
            return;
        }

        // 2. Check the person's true fibrosis state and use it to search for
        // the input table to grab only test characteristics for this state.
        std::unordered_map<std::string, std::string> selectCriteria;
        selectCriteria["true_fib"] = Person::Person::
            fibrosisStateEnumToStringMap[person->getFibrosisState()];
        auto resultTable = this->table->selectWhere(selectCriteria);

        // 3. Get a vector of the probabilities of each of the possible fibrosis
        // outcomes (test one).
        std::vector<double> probs =
            getTransitions(resultTable, "fibrosis_staging.test_one");

        // 4. Decide which stage is assigned to the person.
        Person::MeasuredFibrosisState stateOne =
            (Person::MeasuredFibrosisState)this->getDecision(probs);

        // 5. Assign this value as the person's measured state.
        person->setMeasuredFibrosisState(stateOne);
        this->addStagingCost(person);

        // 6. Get a vector of the probabilities of each of the possible fibrosis
        // outcomes (test two) provided there is a second test.
        probs = getTransitions(resultTable, "fibrosis_staging.test_two");

        // if person is exposed to LTFU, they are not exposed to the second
        // fibtest
        if (!probs.empty()) {
            if (person->exposedToLTFU()) {
                person->setHadFibTestTwo(false);
                return;
            } else {
                person->setExposedToLTFU(true);
            }
        }

        // 7. Decide which stage is assigned to the person.
        if (!probs.empty()) {
            person->setHadFibTestTwo(true);

            Person::MeasuredFibrosisState stateTwo =
                (Person::MeasuredFibrosisState)this->getDecision(probs);

            // determine whether to use latest test value or greatest
            std::string method = std::get<std::string>(this->config.get(
                "fibrosis_staging.multitest_result_method", ""));

            Person::MeasuredFibrosisState measured;
            if (method == "latest") {
                measured = stateTwo;
            } else if (method == "maximum") {
                measured =
                    std::max<Person::MeasuredFibrosisState>(stateOne, stateTwo);
            } else {
                // log an error
                return;
            }
            // 8. Assign this state to the person.
            person->setMeasuredFibrosisState(measured);
            this->addStagingCost(person, true);
        } else {
            // either means the name provided is incorrect or there is no second
            // test
            std::shared_ptr<Data::ReturnType> testTwo =
                this->config.get_optional("fibrosis_staging.test_two", "");
            if (testTwo) {
                // log an error
            }
        }
    }

    std::vector<double>
    FibrosisStaging::getTransitions(Data::IDataTablePtr table,
                                    std::string configLookupKey) {
        std::shared_ptr<Data::ReturnType> stageTest =
            this->config.get_optional(configLookupKey, "");
        if (stageTest) {
            std::vector<std::string> testColumn =
                table->getColumn(std::get<std::string>(*stageTest));
            std::vector<double> probs;
            for (std::string c : testColumn) {
                probs.push_back(stod(c));
            }
            return probs;
        } else {
            return {};
        }
    }

    void FibrosisStaging::addStagingCost(std::shared_ptr<Person::Person> person,
                                         const bool testTwo) {
        double cost = testTwo ? this->testOneCost : this->testTwoCost;
        std::string costName = "Fibrosis Staging Cost (Test" +
                               ((std::string)(testTwo ? "1" : "2")) + ")";
        Cost::Cost fibrosisStagingCost = {this->costCategory, costName, cost};
        person->addCost(fibrosisStagingCost, this->getCurrentTimestep());
    }
} // namespace Event

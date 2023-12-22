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

        uint32_t period = this->config.get<uint32_t>("fibrosis_staging.period");
        if (((uint32_t)timeSinceStaging) < period) {
            return;
        }

        std::unordered_map<std::string, std::string> selectCriteria;
        selectCriteria["true_fib"] =
            Person::Person::liverStateEnumToStringMap[person->getLiverState()];
        auto resultTable = this->table->selectWhere(selectCriteria);

        // 3. Get a vector of the probabilities of each of the possible fibrosis
        // outcomes.
        std::vector<double> probs =
            getTransitions(resultTable, "fibrosis_staging.test_one");

        // 4. Decide which stage is assigned to the person.
        Person::MeasuredLiverState stateOne =
            (Person::MeasuredLiverState)this->getDecision(probs);

        // 3. Get a vector of the probabilities of each of the possible fibrosis
        // outcomes.
        probs = getTransitions(resultTable, "fibrosis_staging.test_two");

        // 4. Decide which stage is assigned to the person.
        if (!probs.empty()) {
            Person::MeasuredLiverState stateTwo =
                (Person::MeasuredLiverState)this->getDecision(probs);
        }

        // 5. Assign this state to the person.
        person->setMeasuredLiverState(stateOne);
    }

    std::vector<double>
    FibrosisStaging::getTransitions(Data::IDataTablePtr table,
                                    std::string configLookupKey) {

        std::shared_ptr<std::string> stageTest =
            this->config.optional<std::string>(configLookupKey);
        if (stageTest) {
            std::vector<std::string> testColumn = table->getColumn(*stageTest);
            std::vector<double> probs;
            for (std::string c : testColumn) {
                probs.push_back(stod(c));
            }
            return probs;
        } else {
            return {};
        }
    }
} // namespace Event

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
/// This file contains the implementation of the BehaviorChanges Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "BehaviorChanges.hpp"

namespace Event {
    void BehaviorChanges::doEvent(std::shared_ptr<Person::Person> person) {
        if (!person->getIsAlive()) {
            return;
        }
        // Determine person's current behavior classification
        Person::BehaviorClassification bc = person->getBehaviorClassification();

        // Insert person's behavior cost
        this->insertBehaviorCost(person);

        // Typical Behavior Change
        // 1. Generate the transition probabilities based on the starting state
        std::vector<double> probs = getTransitions(person);
        // currently using placeholders to test compiling
        // std::vector<double> probs = {0.25, 0.25, 0.25, 0.25};
        // 2. Draw a behavior state to be transitioned to
        Person::BehaviorClassification toBC =
            (Person::BehaviorClassification)this->getDecision(probs);
        // 3. If the drawn state differs from the current state, change the
        // bools in BehaviorState to match
        person->updateBehavior(toBC, this->getCurrentTimestep());
    }

    std::vector<double>
    BehaviorChanges::getTransitions(std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        // intentional truncation
        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        // selectCriteria["moud"] =
        //     Person::Person::moudEnumToStringMap[person->getMoudState()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];

        auto resultTable = table->selectWhere(selectCriteria);

        std::vector<double> result = {};

        std::vector<std::string> columnVec = resultTable->getColumnNames();
        for (auto kv : Person::Person::behaviorClassificationEnumToStringMap) {
            auto res = (*resultTable)[kv.second];
            result.push_back(std::stod(res[0]));
        }
        return result;
    }

    void BehaviorChanges::insertBehaviorCost(
        std::shared_ptr<Person::Person> person) {

        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        // selectCriteria["moud"] =
        //     Person::Person::moudEnumToStringMap[person->getMoudState()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];

        // should reduce to a single value
        auto resultTable = table->selectWhere(selectCriteria);

        if (resultTable->empty()) {
            // log error
            return;
        }

        auto res = (*resultTable)["cost"];
        double cost = std::stod(res[0]);

        Cost::Cost behaviorCost = {this->costCategory, "Drug Behavior", cost};

        person->addCost(behaviorCost, this->getCurrentTimestep());
    }
} // namespace Event

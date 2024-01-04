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
        // Determine person's current behavior classification
        Person::BehaviorClassification bc = person->getBehaviorClassification();

        // MOUD
        // positioned before transitioning to use so that people do not start
        // treatment the same time they become an opioid abuser.
        // Can only enter MOUD if in an active use state.
        // if (!(bc >= Person::BehaviorClassification::NONINJECTION)) {
        //     // 1. Check the person's current MOUD status
        //     Person::MOUD moud = person->getMoudState();
        //     // 2. Draw probability of changing MOUD state.
        //     std::vector<double> probs = {0.50, 0.25, 0.25};
        //     // 3. Make a transition decision.
        //     Person::MOUD toMoud = (Person::MOUD)this->getDecision(probs);
        //     // 4. If the person stays on MOUD, increment their time on MOUD.
        //     // Otherwise, set or keep their time on MOUD as 0.
        //     if ((moud == Person::MOUD::CURRENT) && (moud == toMoud)) {
        //         // increment timeOnMoud
        //     } else {
        //         // assign time spent on MOUD to 0
        //     }
        // }

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
        selectCriteria["moud"] =
            Person::Person::moudEnumToStringMap[person->getMoudState()];
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
} // namespace Event

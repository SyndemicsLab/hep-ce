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
/// This file contains the implementation of the Death Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Death.hpp"

namespace Event {
    void Death::doEvent(std::shared_ptr<Person::Person> person) {
        if (person->age >= 1200) {
            this->die(person);
        }

        // "Calculate background mortality rate based on age, gender, and IDU"
        double backgroundMortality = 1.0;

        // Fatal Overdose
        // 1. Check if the person overdosed this timestep. If no, return.
        if (!person->getOverdose()) {
            return;
        }
        // 2. Get fatal OD probability.
        this->getMortalityProbabilities(person);
        // 3. Decide whether the person dies. If not, unset their overdose
        // property.

        double prob = (this->backgroundMortProb * this->smr) +
                      this->fatalOverdoseProb + this->fibrosisDeathProb;
        if (this->getDecision({1.0 - prob, prob})) {
            this->die(person);
        } else {
            person->toggleOverdose();
        }
    }

    void Death::die(std::shared_ptr<Person::Person> person) {
        // do the death thing here
        person->die();
    }

    void Death::getMortalityProbabilities(
        std::shared_ptr<Person::Person> const person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)person->age);
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];
        selectCriteria["fibrosis_state"] =
            Person::Person::liverStateEnumToStringMap[person->getLiverState()];

        auto resultTable = table->selectWhere(selectCriteria);

        this->backgroundMortProb =
            stod((*resultTable)["background_mortality"][0]);

        this->smr = stod((*resultTable)["SMR"][0]);

        if (person->getLiverState() > Person::LiverState::F3) {
            this->fibrosisDeathProb =
                stod((*resultTable)["fib_death_probability"][0]);
        }

        if (person->getOverdose()) {
            // we need to figure out how we're gonna make this time based
            this->fatalOverdoseProb =
                stod((*resultTable)["fatal_overdose_cycle52"][0]);
        }

        // size_t last_index = str.find_last_not_of("0123456789");
        // string result = str.substr(last_index + 1);
    }
} // namespace Event

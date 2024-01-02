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
/// This file contains the implementation of the Infections Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Infections.hpp"

namespace Event {
    void Infections::doEvent(std::shared_ptr<Person::Person> person) {
        // if person is already infected we can't infect them more
        if (person->getHEPCState() != Person::HEPCState::NONE) {
            return;
        }

        // draw new infection probability
        std::vector<double> prob = this->getInfectProb(person);
        // decide whether person is infected; if value == 0, infect
        int value = this->getDecision(prob);
        if (value != 0) {
            return;
        }
        person->infect(this->getCurrentTimestep());
    }

    std::vector<double>
    Infections::getInfectProb(std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];
        auto resultTable = table->selectWhere(selectCriteria);

        double probInfected = std::stod((*resultTable)["incidence"][0]);
        std::vector<double> result = {probInfected, 1 - probInfected};
        return result;
    }
} // namespace Event

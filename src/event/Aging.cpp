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
/// This file contains the implementation of the Aging Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Aging.hpp"

namespace Event {
    void Aging::doEvent(std::shared_ptr<Person::Person> person) {
        person->age++;
        this->addBackgroundCost(person);
    }

    void Aging::addBackgroundCost(std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];

        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty()) {
            // error
            return;
        }
        auto res = (*resultTable)["cost"];
        double cost = std::stod(res[0]);

        Cost::Cost backgroundCost = {this->costCategory, "Background Cost",
                                     cost};

        person->addCost(backgroundCost, this->getCurrentTimestep());
    }

    void Aging::setBackgroundUtility(std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];

        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty()) {
            // error
            return;
        }
        auto res = (*resultTable)["utility"];
        double utility = std::stod(res[0]);

        person->setUtility(Person::UtilityCategory::BACKGROUND, utility);
    }
} // namespace Event

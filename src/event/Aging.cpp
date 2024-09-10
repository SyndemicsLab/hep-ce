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

namespace event {
    void Aging::doEvent(std::shared_ptr<person::Person> person) {
        person->age++;
        this->addBackgroundCostAndUtility(person);
    }

    void
    Aging::addBackgroundCostAndUtility(std::shared_ptr<person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            person::person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            person::person::behaviorEnumToStringMap[person->getBehavior()];

        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty()) {
            // error
            return;
        }
        auto res = resultTable->getColumn("cost");
        if (res.empty()) {
            this->logger->error("No cost avaliable for Aging");
        } else {
            double cost = std::stod(res[0]);
            Cost::Cost backgroundCost = {this->costCategory, "Background Cost",
                                         cost};
            person->addCost(backgroundCost, this->getCurrentTimestep());
        }

        res = resultTable->getColumn("utility");
        if (res.empty()) {
            this->logger->error("No utility avaliable for Aging");
            return;
        }
        double utility = std::stod(res[0]);
        person->setUtility(utility);
    }
} // namespace event

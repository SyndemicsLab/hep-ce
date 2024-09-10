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

namespace event {
    void BehaviorChanges::doEvent(std::shared_ptr<person::Person> person) {
        // Determine person's current behavior classification
        person::Behavior bc = person->getBehavior();

        // Insert person's behavior cost
        this->calculateCostAndUtility(person);

        // Typical Behavior Change
        // 1. Generate the transition probabilities based on the starting state
        std::vector<double> probs = getTransitions(person);
        // currently using placeholders to test compiling
        // std::vector<double> probs = {0.25, 0.25, 0.25, 0.25};
        // 2. Draw a behavior state to be transitioned to
        int res = this->getDecision(probs);
        if (res >= (int)person::Behavior::COUNT) {
            this->logger->error("Behavior Classification Decision returned "
                                "value outside bounds");
            return;
        }
        person::Behavior toBC = (person::Behavior)res;

        // 3. If the drawn state differs from the current state, change the
        // bools in BehaviorState to match
        person->updateBehavior(toBC, this->getCurrentTimestep());
    }

    std::vector<double>
    BehaviorChanges::getTransitions(std::shared_ptr<person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        // intentional truncation
        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            person::person::sexEnumToStringMap[person->getSex()];
        // selectCriteria["moud"] =
        //     person::person::moudEnumToStringMap[person->getMoudState()];
        selectCriteria["drug_behavior"] =
            person::person::behaviorEnumToStringMap[person->getBehavior()];

        auto resultTable = table->selectWhere(selectCriteria);

        std::vector<double> result = {};

        std::vector<std::string> columnVec = resultTable->getColumnNames();
        for (auto kv : person::person::behaviorEnumToStringMap) {
            auto res = (*resultTable)[kv.second];
            result.push_back(std::stod(res[0]));
        }
        return result;
    }

    void BehaviorChanges::calculateCostAndUtility(
        std::shared_ptr<person::Person> person) const {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            person::person::sexEnumToStringMap[person->getSex()];
        // selectCriteria["moud"] =
        //     person::person::moudEnumToStringMap[person->getMoudState()];
        selectCriteria["drug_behavior"] =
            person::person::behaviorEnumToStringMap[person->getBehavior()];

        // should reduce to a single value
        auto resultTable = table->selectWhere(selectCriteria);

        if (resultTable->empty()) {
            // log error
            return;
        }

        auto res = resultTable->getColumn("cost");
        if (res.empty()) {
            this->logger->error("No cost avaliable for Behavior Changes");
        } else {
            double cost = std::stod(res[0]);
            Cost::Cost behaviorCost = {this->costCategory, "Drug Behavior",
                                       cost};
            person->addCost(behaviorCost, this->getCurrentTimestep());
        }

        res = resultTable->getColumn("utility");
        if (res.empty()) {
            this->logger->error("No utility avaliable for Behavior Changes");
            return;
        }
        double util = std::stod(res[0]);
        person->setUtility(util);
    }
} // namespace event

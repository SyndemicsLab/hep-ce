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
/// This file contains the implementation of the Linking Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Linking.hpp"

namespace event {
    void Linking::doEvent(std::shared_ptr<person::Person> person) {
        person::HCV state = person->getHCV();
        if (state == person::HCV::NONE) {
            // add false positive cost
            person->unlink(this->getCurrentTimestep());
            this->addLinkingCost(person, "False Positive Linking Cost",
                                 this->falsePositiveCost);
            return;
        }

        if (!person->isIdentifiedAsInfected()) {
            person->identifyAsInfected(this->getCurrentTimestep());
        }

        std::vector<double> probs;
        if (person->getLinkageType() == person::LinkageType::BACKGROUND) {
            // link probability
            probs = getTransitions(person, "background_link_probability");
        } else {
            // add intervention cost
            this->addLinkingCost(person, "Intervention Linking Cost",
                                 this->interventionCost);
            // link probability
            probs = getTransitions(person, "intervention_link_probability");
        }

        if (person->getLinkState() == person::LinkageState::UNLINKED) {
            // scale by relink multiplier
            double relinkScalar = std::get<double>(
                this->config.get("linking.relink_multiplier", 1.0));
            probs[1] = probs[1] * relinkScalar;
            probs[0] = 1 - probs[1];
        }

        // draw from link probability
        bool doLink = (bool)this->getDecision(probs);

        if (doLink) {
            // need to figure out how to pass in the LinkageType to the event
            person->link(this->getCurrentTimestep(), person->getLinkageType());
        } else if (!doLink &&
                   person->getLinkState() == person::LinkageState::LINKED) {
            person->unlink(this->getCurrentTimestep());
        }
    }

    std::vector<double>
    Linking::getTransitions(std::shared_ptr<person::Person> person,
                            std::string columnKey) {
        std::unordered_map<std::string, std::string> selectCriteria;

        // intentional truncation
        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            person::person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            person::person::behaviorEnumToStringMap[person->getBehavior()];

        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty()) {
            // error
            return {};
        }

        std::vector<std::string> col = resultTable->getColumn(columnKey);

        std::vector<double> doubleVector(col.size());
        std::transform(col.begin(), col.end(), doubleVector.begin(),
                       [](const std::string &val) { return std::stod(val); });

        // {0: don't link, 1: link}
        std::vector<double> result = {1 - doubleVector[0], doubleVector[0]};

        return result;
    }

    void Linking::addLinkingCost(std::shared_ptr<person::Person> person,
                                 std::string name, double cost) {
        Cost::Cost linkingCost = {this->costCategory, name, cost};
        person->addCost(linkingCost, this->getCurrentTimestep());
    }
} // namespace event

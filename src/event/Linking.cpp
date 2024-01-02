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

namespace Event {
    void Linking::doEvent(std::shared_ptr<Person::Person> person) {
        Person::HEPCState state = person->getHEPCState();
        if (state == Person::HEPCState::NONE) {
            // add false positive cost
            person->unlink(this->getCurrentTimestep());
            return;
        }

        if (!person->isIdentifiedAsInfected()) {
            person->identifyAsInfected(this->getCurrentTimestep());
        }

        std::vector<double> probs;
        if (person->getLinkageType() == Person::LinkageType::BACKGROUND) {
            // link probability
            probs = getTransitions(person, "background_linking");
        } else {
            // add intervention cost
            // link probability
            probs = getTransitions(person, "intervention_linking");
        }

        if (person->getLinkState() == Person::LinkageState::UNLINKED) {
            // scale by relink multiplier
            double relinkScalar =
                this->config.get<double>("linking.relink_multiplier");
            probs[1] = probs[1] * relinkScalar;
            probs[0] = 1 - probs[1];
        }

        // draw from link probability
        bool doLink = (bool)this->getDecision(probs);

        if (doLink) {
            // need to figure out how to pass in the LinkageType to the event
            person->link(this->getCurrentTimestep(), person->getLinkageType());
        } else if (!doLink &&
                   person->getLinkState() == Person::LinkageState::LINKED) {
            person->unlink(this->getCurrentTimestep());
        }
    }

    std::vector<double>
    Linking::getTransitions(std::shared_ptr<Person::Person> person,
                            std::string columnKey) {
        std::unordered_map<std::string, std::string> selectCriteria;

        // intentional truncation
        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];

        auto resultTable = table->selectWhere(selectCriteria);

        std::vector<std::string> col = resultTable->getColumn(columnKey);

        std::vector<double> doubleVector(col.size());
        std::transform(col.begin(), col.end(), doubleVector.begin(),
                       [](const std::string &val) { return std::stod(val); });

        // {0: don't link, 1: link}
        std::vector<double> result = {1 - doubleVector[0], doubleVector[0]};

        return result;
    }

} // namespace Event

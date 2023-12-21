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
/// This file contains the implementation of the DiseaseProgression Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "DiseaseProgression.hpp"

namespace Event {
    void DiseaseProgression::doEvent(std::shared_ptr<Person::Person> person) {
        // can only progress in fibrosis state if actively infected with HCV
        // for people in F3 or later, there is still a chance of HCC progression
        if (person->getHEPCState() == Person::HEPCState::NONE) {
            if (person->getLiverState() < Person::LiverState::F3) {
                return;
            }
        }
        // 1. Get current disease status
        Person::LiverState ls = person->getLiverState();
        // 2. Get the transition probabilities from that state
        std::vector<double> probs = getTransitions(person);
        // currently using placeholders to test compiling
        // std::vector<double> probs = {0.2, 0.2, 0.2, 0.2, 0.2};
        // 3. Randomly draw the state to transition to
        Person::LiverState toLS = (Person::LiverState)this->getDecision(probs);
        // 4. Transition to the new state
        person->updateLiver(toLS, this->getCurrentTimestep());
    }

    std::vector<double>
    DiseaseProgression::getTransitions(std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        // intentional truncation
        selectCriteria["initial_state"] =
            Person::Person::liverStateEnumToStringMap[person->getLiverState()];
        auto resultTable = table->selectWhere(selectCriteria);
        std::vector<std::string> dataTable =
            resultTable->getColumn("probability");

        std::vector<double> result = {0.0};
        for (std::string row : dataTable) {
            result.push_back(std::stod(row));
        }
        return result;
    }
} // namespace Event

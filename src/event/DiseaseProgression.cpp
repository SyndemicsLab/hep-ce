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
        // 1. Get current disease status
        Person::LiverState ls = person->getLiverState();
        // 2. Get the transition probabilities from that state
        // std::vector<double> probs = someFunction(ls);
        // currently using placeholders to test compiling
        std::vector<double> probs = {0.2, 0.2, 0.2, 0.2, 0.2};
        // 3. Randomly draw the state to transition to
        Person::LiverState toLS = (Person::LiverState)this->getDecision(probs);
        // 4. Transition to the new state
        person->updateLiver(toLS);
    }
} // namespace Event

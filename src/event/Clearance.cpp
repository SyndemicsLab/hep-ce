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
/// This file contains the implementation of the Clearance Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Clearance.hpp"

namespace Event {
    void Clearance::doEvent(std::shared_ptr<Person::Person> person) {
        // people infected with hcv have some probability of spontaneous
        // clearance.

        // if person isn't infected, nothing to do
        if (person->getHEPCState() == Person::HEPCState::NONE) {
            return;
        }
        // 1. Get the probability of acute clearance
        std::vector<double> prob = this->getClearanceProb();
        // 2. Decide whether the person clears
        int value =  this->getDecision(prob);
        if (!value) {
            return;
        }
        person->clearHCV();
    }

    std::vector<double> Clearance::getClearanceProb() {
        // probabilityToRate doesn't include time, hence division by 6.0
        return {Utils::probabilityToRate(0.25)/6.0};
    }
} // namespace Event

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

        if (person->getLinkageType() == Person::LinkageType::BACKGROUND) {
            // link probability
        } else {
            // add intervention cost
            // link probability
        }

        if (person->getLinkState() == Person::LinkageState::UNLINKED) {
            // scale by relink multiplier
        }

        // draw from link probability
        bool value = false;

        if (value) {
            // need to figure out how to pass in the LinkageType to the event
            person->link(this->getCurrentTimestep(),
                         Person::LinkageType::BACKGROUND);
        } else if (!value &&
                   person->getLinkState() == Person::LinkageState::LINKED) {
            person->unlink(this->getCurrentTimestep());
        }
    }

} // namespace Event

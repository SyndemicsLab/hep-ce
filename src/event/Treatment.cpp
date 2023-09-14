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
/// This file contains the implementation of the Treatment Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Treatment.hpp"

namespace Event {
    void Treatment::doEvent(std::shared_ptr<Person::Person> person) {
        Person::LiverState personLiverState = person->getLiverState();
        if (person->getHEPCState() == Person::HEPCState::NONE &&
            personLiverState == Person::LiverState::NONE) {
            return;
        }

        if (personLiverState == Person::LiverState::DECOMP ||
            personLiverState == Person::LiverState::F4) {
            // assign cirrhotic state?
        } else {
            // assign non-cirrhotic state?
        }
        // Need to do stuff for building a treatment
    }
} // namespace Event

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
/// This file contains the implementation of the Fibrosis Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Fibrosis.hpp"

namespace Event {
    void Fibrosis::doEvent(std::shared_ptr<Person::Person> person) {
        // 1. Check the time since the person's last fibrosis staging test. If
        // the person's last test is more recent than the limit, exit event.
        // 2. Check the person's true liver state.
        // 3. Get a vector of the probabilities of each of the possible fibrosis
        // outcomes.
        // 4. Decide which stage is assigned to the person.
        // 5. Assign this state to the person.
    }
} // namespace Event

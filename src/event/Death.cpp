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
/// This file contains the implementation of the Death Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Death.hpp"

namespace Event {
    void Death::doEvent(std::shared_ptr<Person::Person> person) {
        if (person->age >= 1200) {
            this->die(person);
        }

        // "Calculate background mortality rate based on age, gender, and IDU"
        double backgroundMortality = 1.0;

        // Fatal Overdose
        // 1. Check if the person overdosed this timestep.
        // 2. If no, return. Otherwise, get fatal OD probability.
        // 3. Decide whether the person dies. If not, unset their overdose
        // property.
    }

    void Death::die(std::shared_ptr<Person::Person> person) {
        // do the death thing here
        person->die();
    }
} // namespace Event

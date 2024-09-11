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

#include "Overdose.hpp"

namespace event {
    void Overdose::doEvent(person::PersonBase &person) {
        person::Behavior bc = person->getBehavior();
        // return immediately if not in active use state
        if (bc < person::Behavior::NONINJECTION) {
            return;
        }
        // check od probability
        double overdoseProbability = this->getProbability(person);
        // determine if person overdoses
        if (this->getDecision(
                {1.0 - overdoseProbability, overdoseProbability})) {
            person->toggleOverdose();
        }
    }

    double Overdose::getProbability(person::PersonBase &person) {
        // overdose probability is stratified by behavior classification and
        // MOUD state
        return 0.0;
    }
} // namespace event

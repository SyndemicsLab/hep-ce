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
/// This file contains the implementation of the MOUD Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "MOUD.hpp"

namespace Event {
    void MOUD::doEvent(std::shared_ptr<Person::Person> person) {
        Person::Behavior bc = person->getBehavior();

        // Can only enter MOUD if in an active use state.
        if (!(bc >= Person::Behavior::NONINJECTION)) {
            // 1. Check the person's current MOUD status
            Person::MOUD moud = person->getMoudState();
            // 2. Draw probability of changing MOUD state.
            // TODO: MAKE THIS A REAL TRANSITION RATE
            std::vector<double> probs = {0.50, 0.25, 0.25};
            // 3. Make a transition decision.
            int res = this->getDecision(probs);
            if (res >= (int)Person::MOUD::COUNT) {
                this->logger->error("MOUD Decision returned "
                                    "value outside bounds");
                return;
            }
            Person::MOUD toMoud = (Person::MOUD)res;
            if (toMoud == Person::MOUD::CURRENT) {
                if (moud != toMoud) {
                    // new treatment start
                    person->setMoudState(toMoud);
                    person->setTimeStartedMoud(this->getCurrentTimestep());
                }
                // person continuing treatment
                // add treatment cost
            } else {
                // person discontinuing treatment
                // or going from post-treatment to no treatment
                person->setMoudState(toMoud);
                // figure out if we want to update timestartedmoud to an
                // impossible value, e.g. -1
            }
        }
    }
} // namespace Event

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
        if (!this->isEligible(person)) {
            return;
        }
        Person::LiverState personLiverState = person->getLiverState();
        if (person->getHEPCState() == Person::HEPCState::NONE &&
            personLiverState == Person::LiverState::NONE) {
            return;
        }
        // Need to do stuff for building a treatment
    }

    bool
    Treatment::isEligible(std::shared_ptr<Person::Person> const person) const {
        Person::LiverState liverState = person->getLiverState();
        int timeSinceLinked = person->getTimeOfLinkChange();
        Person::BehaviorClassification behavior =
            person->getBehaviorClassification();
        int timeBehaviorChange = person->getTimeBehaviorChange();
        if (!isEligibleFibrosisStage(liverState) ||
            ((this->getCurrentTimestep() - timeSinceLinked) >
             eligibleTimeSinceLinked) ||
            (behavior == Person::BehaviorClassification::INJECTION) ||
            (behavior == Person::BehaviorClassification::FORMER_INJECTION &&
             timeBehaviorChange < eligibleTimeBehaviorChange)) {
            return false;
        } else {
            return true;
        }
    }

    bool
    Treatment::isEligibleFibrosisStage(Person::LiverState liverState) const {
        for (Person::LiverState eligibleState : this->eligibleLiverStates) {
            if (liverState < eligibleState) {
                return true;
            }
        }
        return false;
    }

    Course Treatment::getTreatmentCourse(
        std::shared_ptr<Person::Person> const person) const {
        Person::LiverState personLiverState = person->getLiverState();
        if (personLiverState > Person::LiverState::F3) {
            if (person->hadIncompleteTreatment()) {

            } else {
            }
        } else {
            if (person->hadIncompleteTreatment()) {

            } else {
            }
        }
        return {};
    }

} // namespace Event

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
        // 1. Determine person's treatment eligibility.
        if (!this->isEligible(person)) {
            return;
        }

        // accounting for when the person was a false positive -- do these make
        // it this far?

        // Person::LiverState personLiverState = person->getLiverState();
        // if (person->getHEPCState() == Person::HEPCState::NONE &&
        //     personLiverState == Person::LiverState::NONE) {
        //     return;
        // }

        // 2. Draw the probability that person will initiate treatment.
        // 3. Select the treatment course for a person based on their measured
        // fibrosis stage.
        // 4. Check the time since treatment initiation.
        // 5. If time since treatment initiation > 0, draw probability of
        // adverse outcome (TOX), then draw probability of withdrawing from
        // treatment prior to completion. TOX does not lead to withdrawal from
        // care but withdrawal probabilities are stratified by TOX.
        // 6. Compare the treatment duration to the time since treatment
        // initiation. If equal, draw for cure (SVR) vs no cure (EOT).
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

    void Treatment::populateCourses() {
        std::vector<std::string> courseList =
            this->config.getStringVector("treatment.courses");
        for (std::string &course : courseList) {
            std::string subsectionCourses =
                "treatment_" + course + ".components";
            std::vector<std::string> componentList =
                this->config.getStringVector(subsectionCourses);
            for (std::string &component : componentList) {
                std::string subsectionComponent = "treatment_" + component;
                this->config.get<double>(subsectionComponent + ".cost");
            }
        }
    }
} // namespace Event

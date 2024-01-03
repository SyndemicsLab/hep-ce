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
    Treatment::Treatment(std::mt19937_64 &generator, Data::IDataTablePtr table,
                         Data::Configuration &config,
                         std::shared_ptr<spdlog::logger> logger)
        : ProbEvent::ProbEvent(generator, table, config, logger) {
        this->populateCourses();
    }

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
        std::vector<Course> tempCourses = {};
        std::vector<std::string> courseList =
            this->config.getStringVector("treatment.courses");
        // used for tracking section hierarchy
        std::vector<std::string> sections = {"treatment"};
        // error if courseList length != total number of treatment groups
        // total number of treatment groups = 8 (2024-01-03)

        for (std::string &course : courseList) {
            std::vector<std::string> section = {"treatment_" + course};
            section += sections;
            std::vector<Regimen> regimens = {};
            std::string subsectionCourses = section[0] + ".regimens";
            std::vector<std::string> regimenList =
                this->config.getStringVector(subsectionCourses);

            for (std::string &regimen : regimenList) {
                std::vector<Component> components = {};
                std::vector<std::string> sect = {"treatment_" + regimen};
                sect += section;
                std::string subsectionRegimens = sect[0] + ".components";
                std::vector<std::string> componentList =
                    this->config.getStringVector(subsectionRegimens);

                for (std::string &component : componentList) {
                    std::vector<std::string> sec = {"treatment_" + component};
                    sec += sect;
                    Component comp = {component,
                                      this->locateInput(sec, "cost")};
                    components.push_back(comp);
                }

                Regimen reg = {components,
                               (int)this->locateInput(sect, "duration")};
                regimens.push_back(reg);
            }

            Course cour = {regimens, (int)locateInput(section, "duration"), 0.0,
                           0.0};
            tempCourses.push_back(cour);
        }
        this->courses = tempCourses;
    }

    double Treatment::locateInput(std::vector<std::string> &configSections,
                                  const std::string &parameter) {
        for (auto section : configSections) {
            std::shared_ptr<double> param =
                this->config.optional<double>(section + '.' + parameter);
            if (param) {
                return *param;
            }
        }
        // error, value not specified
        return -1;
    }
} // namespace Event

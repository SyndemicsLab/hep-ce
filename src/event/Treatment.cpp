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
                         std::shared_ptr<spdlog::logger> logger,
                         std::string name)
        : ProbEvent::ProbEvent(generator, table, config, logger, name) {
        this->populateCourses();
    }

    void Treatment::doEvent(std::shared_ptr<Person::Person> person) {
        // 1. Determine person's treatment eligibility.
        if (!this->isEligible(person)) {
            return;
        }

        // 2. Select the treatment course for a person based on their measured
        // fibrosis stage, genotype.
        const Course &course = this->getTreatmentCourse(person);
        // 3. If the person has not yet initiated treatment, draw the
        // probability that person will initiate treatment.
        if (!person->hasInitiatedTreatment()) {
            int initiation = this->getDecision({course.initiationProbability});
            // if the randomly-drawn value from getDecision is 0, person does
            // not initiate treatment
            if (initiation == 0) {
                return;
            }
            // person initiates treatment -- set treatment initiation values
            person->setInitiatedTreatment(true);
            person->setTimeOfTreatmentInitiation(this->getCurrentTimestep());
        }
        // 4. Check the time since treatment initiation.
        int timeSinceInitiation =
            this->getCurrentTimestep() - person->getTimeOfTreatmentInitiation();
        // 5. If time since treatment initiation > 0, draw probability of
        // adverse outcome (TOX), then draw probability of withdrawing from
        // treatment prior to completion. TOX does not lead to withdrawal from
        // care but withdrawal probabilities are stratified by TOX.
        if (timeSinceInitiation == 0) {
            return;
        }
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
        const Person::LiverState &personLiverState = person->getLiverState();
        if (personLiverState > Person::LiverState::F3) {
        } else {
        }
        return {};
    }

    void Treatment::populateCourses() {
        std::vector<Course> tempCourses = {};
        std::vector<std::string> courseList =
            this->config.getStringVector("treatment.courses");
        // error if courseList length != total number of treatment groups
        // total number of treatment groups = 4 (2024-01-03)

        // used for tracking section hierarchy
        std::vector<std::string> sections = {"treatment"};

        for (std::string &course : courseList) {
            // local variable for sections relevant to courses
            std::vector<std::string> courseSections =
                setupTreatmentSections({"course_" + course}, sections);
            // setting up regimens for the current course
            std::vector<Regimen> regimens = {};
            std::vector<std::string> regimenList =
                this->config.getStringVector(courseSections[0] + ".regimens");
            for (std::string &regimen : regimenList) {
                std::vector<std::string> regimenSections =
                    setupTreatmentSections({"regimen_" + regimen},
                                           courseSections);
                Regimen currentRegimen = {
                    (int)this->locateInput(regimenSections, "duration"),
                    this->locateInput(regimenSections, "cost"),
                    this->locateInput(regimenSections, "utility"),
                    this->locateInput(regimenSections,
                                      "withdrawal_probability"),
                    this->locateInput(regimenSections, "svr_probability"),
                    this->locateInput(regimenSections, "tox_probability"),
                    this->locateInput(regimenSections, "tox_cost"),
                    this->locateInput(regimenSections, "tox_utility")};
                regimens.push_back(currentRegimen);
            }

            Course currentCourse = {
                regimens,
                this->locateInput(courseSections, "initiation_probability")};
            tempCourses.push_back(currentCourse);
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
        // throw std::runtime_error
        return -1;
    }
} // namespace Event

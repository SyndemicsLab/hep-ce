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
                         Data::Config &config,
                         std::shared_ptr<spdlog::logger> logger,
                         std::string name)
        : ProbEvent::ProbEvent(generator, table, config, logger, name) {
        this->costCategory = Cost::CostCategory::TREATMENT;
        this->populateCourses();
    }

    void Treatment::doEvent(std::shared_ptr<Person::Person> person) {
        if (!person->getIsAlive()) {
            return;
        }

        // 1. Determine person's treatment eligibility. Bypass this if person
        // has already initiated treatment.
        if (!person->hasInitiatedTreatment()) {
            if (!this->isEligible(person)) {
                // check if person is lost to follow-up due to ineligibility
                this->checkLossToFollowUp(person);
                return;
            }
            if (person->getTreatmentCount() > 0) {
                double rt_prob = std::get<double>(
                    this->config.get("treatment.retreatment_probability", 1.0));
                int retreat = this->getDecision({rt_prob});
                if (retreat == 0) {
                    return;
                }
            }
        }

        // 2. Check loss to follow-up, should be due to second fib test.
        if (person->exposedToLTFU() && !person->hadFibTestTwo()) {
            // only pull once after condition is met
            person->setExposedToLTFU(false);
            this->checkLossToFollowUp(person);
            return;
        }

        // 3. Select the treatment course for a person based on their measured
        // fibrosis stage, genotype.
        const Course &course = this->getTreatmentCourse(person);
        // 4. If the person has not yet initiated treatment, draw the
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
            // add to treatment initiation count
            person->incrementTreatCount();
            // return, as a person's state doesn't change the timestep they
            // start treatment
            return;
        }
        // 5. Check the time since treatment initiation.
        int timeSinceInitiation =
            this->getCurrentTimestep() - person->getTimeOfTreatmentInitiation();
        // 6. Get the treatment regimen the person is currently experiencing.
        const std::pair<Regimen, int> &regimenInfo =
            this->getCurrentRegimen(course, timeSinceInitiation);
        // accumulate costs
        this->addTreatmentCost(person, regimenInfo.first.cost);
        // set treatment utility
        person->setUtility(Person::UtilityCategory::TREATMENT,
                           regimenInfo.first.utility);
        // 7. If time since treatment initiation > 0, draw probability of
        // adverse outcome (TOX), then draw probability of withdrawing from
        // treatment prior to completion. TOX does not lead to withdrawal from
        // care but withdrawal probabilities are stratified by TOX.
        double withdrawalProbability = regimenInfo.first.withdrawalProbability;
        int toxicity =
            this->getDecision({regimenInfo.first.toxicityProbability});
        if (toxicity == 1) {
            // log toxicity for person
            // apply toxicity cost and utility
            this->addTreatmentCost(person, regimenInfo.first.toxicityCost);
            // add to toxicity count for person
            person->addTox();
            // adjust withdrawalProbability
        }
        int withdraw = this->getDecision({withdrawalProbability});
        if (withdraw == 1) {
            person->setInitiatedTreatment(false);
            // add to withdrawal count for person
            person->addWithdrawal();
            // reset utility
            person->setUtility(Person::UtilityCategory::TREATMENT, 1.0);
            return;
        }
        // 8. Compare the treatment duration to the time since treatment
        // initiation. If equal, draw for cure (SVR) vs no cure (EOT).
        if (timeSinceInitiation == regimenInfo.second) {
            int treatmentOutcome =
                this->getDecision({regimenInfo.first.svrProbability});
            // add EOT for person
            person->addEOT();
            // reset utility
            person->setUtility(Person::UtilityCategory::TREATMENT, 1.0);
            if (treatmentOutcome == 0) {
                // log EOT without cure
                // reached EOT without cure
                person->setIncompleteTreatment(true);
                return;
            }
            // cure
            // add SVR count to person
            person->addSVR();
            // log person cured
            // cured people remove half their hcv cost
            person->clearHCV(false);
        }
    }

    std::pair<Regimen, int> Treatment::getCurrentRegimen(const Course &course,
                                                         int duration) {
        int totalDuration = 0;
        for (const Regimen &curr : course.regimens) {
            totalDuration += curr.duration;
            if (duration <= totalDuration) {
                return {curr, totalDuration};
            }
        }
        // error
        return {Regimen(), 0};
    }

    bool
    Treatment::isEligible(std::shared_ptr<Person::Person> const person) const {
        Person::FibrosisState fibrosisState = person->getFibrosisState();
        int timeSinceLinked = person->getTimeOfLinkChange();
        Person::BehaviorClassification behavior =
            person->getBehaviorClassification();
        int timeBehaviorChange = person->getTimeBehaviorChange();
        if (!isEligibleFibrosisStage(fibrosisState) ||
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

    bool Treatment::isEligibleFibrosisStage(
        Person::FibrosisState fibrosisState) const {
        for (Person::FibrosisState eligibleState :
             this->eligibleFibrosisStates) {
            if (fibrosisState < eligibleState) {
                return true;
            }
        }
        return false;
    }

    Course Treatment::getTreatmentCourse(
        const std::shared_ptr<Person::Person> person) const {
        const Person::FibrosisState &personFibrosisState =
            person->getFibrosisState();
        // check person's infection genotype
        // if gt3, start at index 3 instead of 0
        int idx = person->getGenotype() ? 3 : 0;

        if (personFibrosisState > Person::FibrosisState::F4) {
            // cirrhotic
            if (personFibrosisState == Person::FibrosisState::F4) {
                // compensated
                idx += 1;
            } else {
                // decompensated
                idx += 2;
            }
        }

        return this->courses[idx];
    }

    void Treatment::populateCourses() {
        std::vector<Course> tempCourses = {};
        std::vector<std::string> courseList =
            this->config.getStringVector("treatment.courses");
        // error if courseList length != total number of treatment groups
        // total number of treatment groups = 6 (2024-02-20)
        if (courseList.size() != 6) {
            // log error
            return;
        }

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
            std::shared_ptr<Data::ReturnType> param =
                this->config.get_optional(section + '.' + parameter, -1.0);
            if (param) {
                return std::get<double>(*param);
            }
        }
        // error, value not specified
        // throw std::runtime_error
        return -1;
    }

    void Treatment::addTreatmentCost(std::shared_ptr<Person::Person> person,
                                     double cost) {
        Cost::Cost treatmentCost = {this->costCategory, "Treatment Cost", cost};
        person->addCost(treatmentCost, this->getCurrentTimestep());
    }

    void
    Treatment::checkLossToFollowUp(std::shared_ptr<Person::Person> person) {
        double ltfuProb =
            std::get<double>(this->config.get("treatment.ltfu_prob", 0.0));
        int ltfu = this->getDecision({ltfuProb});
        if (ltfu == 0) {
            // unlink
            person->unlink(this->getCurrentTimestep());
        }
    }
} // namespace Event

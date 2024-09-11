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

namespace event {
    Treatment::Treatment(std::mt19937_64 &generator, Data::IDataTablePtr table,
                         Data::Config &config,
                         std::shared_ptr<spdlog::logger> logger,
                         std::string name)
        : ProbEvent::ProbEvent(generator, table, config, logger, name) {
        this->costCategory = Cost::CostCategory::TREATMENT;
    }

    void Treatment::doEvent(person::PersonBase &person) {
        // 1. Check if the Person is Lost To Follow Up (LTFU)
        if (this->isLostToFollowUp(person)) {
            this->quitEngagement(person);
            return;
        }

        // 2. Charge the Cost of the Visit (varies if this is retreatment)
        chargeCostOfVisit(person);

        // 3. Determine if the Person Engages and Initiates Treatment (i.e.
        // picks up medicine)
        if (!this->initiatesTreatment(person)) {
            this->quitEngagement(person);
            return;
        }

        // 4. Select the treatment course for a person based on their measured
        // fibrosis stage, genotype.
        std::unordered_map<std::string, std::string> selectCriteria = {};
        selectCriteria["is_genotype3"] =
            (person->isGenotypeThree()) ? "TRUE" : "FALSE";
        selectCriteria["is_cirrhotic"] =
            (person->isCirrhotic()) ? "TRUE" : "FALSE";
        Data::IDataTablePtr course = this->table->selectWhere(selectCriteria);

        // 5. Charge the person for the Course they are on
        chargeCostOfCourse(person, course);

        // 6. Determine if the person withdraws from the treatment
        if (this->doesWithdraw(person, course)) {
            this->quitEngagement(person);
            return;
        }

        // 7. Determine if the person has a toxic reaction
        if (this->experiencedToxicity(person, course)) {
            this->quitEngagement(person);
            Cost::Cost toxicityCost = {
                this->costCategory, "Toxicity Cost",
                std::get<double>(this->config.get("treatment.tox_cost", 0.0))};
            person->addCost(toxicityCost, this->getCurrentTimestep());
            person->setUtility(std::get<double>(
                this->config.get("treatment.tox_utility", 0.0)));
        }

        // 8. Determine if the person has been treated long enough, if they
        // achieve SVR
        int timeSinceInitiation =
            this->getCurrentTimestep() - person->getTimeOfTreatmentInitiation();
        int treatmentTime = std::stod(course->getColumn("time")[0]);
        double svr = std::stod(course->getColumn("svr")[0]);
        int treatmentOutcome = this->getDecision({svr});
        if (treatmentOutcome == 1) {
            person->clearHCV(this->getCurrentTimestep());
        }
        if (timeSinceInitiation >= treatmentTime) {
            this->quitEngagement(person);
        }
    }

    bool
    Treatment::isEligible(std::shared_ptr<person::Person> const person) const {
        person::FibrosisState fibrosisState = person->getFibrosisState();
        int timeSinceLinked = person->getTimeOfLinkChange();
        person::Behavior behavior = person->getBehavior();
        int timeBehaviorChange = person->getTimeBehaviorChange();
        person::PregnancyState pregnancyState = person->getPregnancyState();
        if (!isEligibleFibrosisStage(fibrosisState) ||
            ((this->getCurrentTimestep() - timeSinceLinked) >
             eligibleTimeSinceLinked) ||
            (behavior == person::Behavior::INJECTION) ||
            (behavior == person::Behavior::FORMER_INJECTION &&
             timeBehaviorChange < eligibleTimeBehaviorChange) ||
            (pregnancyState == person::PregnancyState::PREGNANT ||
             pregnancyState == person::PregnancyState::POSTPARTUM)) {
            return false;
        } else {
            return true;
        }
    }

    bool Treatment::isEligibleFibrosisStage(
        person::FibrosisState fibrosisState) const {
        for (person::FibrosisState eligibleState :
             this->eligibleFibrosisStates) {
            if (fibrosisState < eligibleState) {
                return true;
            }
        }
        return false;
    }

    void Treatment::addTreatmentCostAndUtility(person::PersonBase &person,
                                               double cost, double util) {
        Cost::Cost treatmentCost = {this->costCategory, "Treatment Cost", cost};
        person->addCost(treatmentCost, this->getCurrentTimestep());
        person->setUtility(util);
    }

    bool Treatment::isLostToFollowUp(person::PersonBase &person) {
        if (!person->hasInitiatedTreatment() && !this->isEligible(person)) {
            // check if person is lost to follow-up due to ineligibility
            double ltfuProb = std::get<double>(
                this->config.get("treatment.ltfu_probability", 0.0));
            int ltfu = this->getDecision({ltfuProb});
            if (ltfu == 0) {
                // unlink
                person->unlink(this->getCurrentTimestep());
                return true;
            }
        }
        return false;
    }

    void Treatment::chargeCostOfVisit(person::PersonBase &person) {
        double cost =
            std::get<double>(this->config.get("treatment.treatment_cost", 0.0));
        Cost::Cost visitCost = {this->costCategory, "Cost of Treatment Visit",
                                cost};
        person->addCost(visitCost, this->getCurrentTimestep());
    }

    void Treatment::chargeCostOfCourse(person::PersonBase &person,
                                       Data::IDataTablePtr course) {

        double cost = std::stod(course->getColumn("treatment_cost")[0]);
        double util = std::get<double>(
            this->config.get("treatment.treatment_utility", 0.0));
        Cost::Cost courseCost = {this->costCategory, "Cost of Treatment Course",
                                 cost};
        person->addCost(courseCost, this->getCurrentTimestep());
        person->setUtility(util);
    }

    bool Treatment::initiatesTreatment(person::PersonBase &person) {
        // if person hasn't initialized draw, if they have, continue treatment
        if (!person->hasInitiatedTreatment()) {
            double initProb = std::get<double>(
                this->config.get("treatment.treatment_initialization", 0.0));
            int initiation = this->getDecision({initProb});
            // if the randomly-drawn value from getDecision is 0, person does
            // not initiate treatment
            if (initiation == 0) {

                return false;
            }
            // person initiates treatment -- set treatment initiation values
            person->setInitiatedTreatment(true);
            person->setTimeOfTreatmentInitiation(this->getCurrentTimestep());
        }
        return true;
    }

    bool Treatment::doesWithdraw(std::shared_ptr<person::Person>,
                                 Data::IDataTablePtr course) {
        double withdrawalProb =
            std::stod(course->getColumn("withdrawal_probability")[0]);

        int withdraw = this->getDecision({withdrawalProb});
        if (withdraw == 1) {
            return true;
        }
        return false;
    }

    bool Treatment::experiencedToxicity(person::PersonBase &person,
                                        Data::IDataTablePtr course) {
        double toxProb = std::stod(course->getColumn("toxicity")[0]);
        int toxicity = this->getDecision({toxProb});
        return (toxicity == 1) ? true : false;
    }

    void Treatment::quitEngagement(person::PersonBase &person) {
        // unlink from care
        person->unlink(this->getCurrentTimestep());
        // reset utility
        person->setUtility(1.0);
    }
} // namespace event

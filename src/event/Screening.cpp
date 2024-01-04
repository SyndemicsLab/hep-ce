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
/// This file contains the implementation of the Screening Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Screening.hpp"

namespace Event {
    void Screening::doEvent(std::shared_ptr<Person::Person> person) {

        // one-time screen or periodic screen
        if ((person->isInterventionScreened() &&
             person->getScreeningFrequency() == -1 &&
             person->getTimeOfLastScreening() == -1) ||
            (person->isInterventionScreened() &&
             person->getScreeningFrequency() != -1 &&
             (this->getCurrentTimestep() - person->getTimeOfLastScreening()) >
                 person->getScreeningFrequency())) {
            std::vector<double> interventionProbability =
                this->getInterventionScreeningProbability(person);
            int choice = getDecision(interventionProbability);
            if (choice == 0) {
                this->interventionScreen(person);
                return;
            }
        }

        // [screen, don't screen]
        std::vector<double> backgroundProbability =
            this->getBackgroundScreeningProbability(person);
        int choice = getDecision(backgroundProbability);
        if (choice == 0) {
            this->backgroundScreen(person);
        }
    }

    void Screening::backgroundScreen(std::shared_ptr<Person::Person> person) {
        if ((this->getCurrentTimestep() - person->getTimeOfLastScreening()) ==
                0 &&
            this->getCurrentTimestep() > 0) {
            return;
        }
        person->markScreened();

        if (!this->antibodyTest(person,
                                std::string("screening_background_ab")) &&
            !this->antibodyTest(person,
                                std::string("screening_background_ab"))) {
            return; // run two tests and if both are negative do nothing
        }

        // if either is positive then...
        if (this->rnaTest(person, std::string("screening_background_rna"))) {
            person->link(this->getCurrentTimestep(),
                         Person::LinkageType::BACKGROUND);
            // what else needs to happen during a link?
        }

        person->unlink(this->getCurrentTimestep());
    }

    void Screening::interventionScreen(std::shared_ptr<Person::Person> person) {
        person->markScreened();
        if (!this->antibodyTest(person,
                                std::string("screening_intervention_ab")) &&
            !this->antibodyTest(person,
                                std::string("screening_intervention_ab"))) {
        }
        if (this->rnaTest(person, std::string("screening_intervention_rna"))) {
            person->link(this->getCurrentTimestep(),
                         Person::LinkageType::INTERVENTION);
            // what else needs to happen during a link?
        } else {
            person->unlink(this->getCurrentTimestep());
        }
    }

    bool Screening::antibodyTest(std::shared_ptr<Person::Person> person,
                                 std::string configKey) {
        double probability = 0.5;
        if (person->getSeropositivity()) {
            Person::HEPCState infectionStatus = person->getHEPCState();
            if (infectionStatus == Person::HEPCState::ACUTE ||
                infectionStatus == Person::HEPCState::NONE) {
                probability =
                    1 -
                    stod(this->config.get(configKey + ".acute_sensitivity"));
            } else {
                probability =
                    1 -
                    stod(this->config.get(configKey + ".chronic_sensitivity"));
            }
        } else {
            probability = stod(this->config.get(configKey + ".specificity"));
        }
        // probability is the chance of false positive or false negative
        int value = getDecision({probability});
        return value;
    }

    bool Screening::rnaTest(std::shared_ptr<Person::Person> person,
                            std::string configKey) {
        double probability = 0.5;
        Person::HEPCState infectionStatus = person->getHEPCState();
        if (infectionStatus == Person::HEPCState::ACUTE) {
            probability =
                1 - stod(this->config.get(configKey + ".acute_sensitivity"));
        } else if (infectionStatus == Person::HEPCState::CHRONIC) {
            probability =
                1 - stod(this->config.get(configKey + ".chronic_sensitivity"));
        } else {
            probability = stod(this->config.get(configKey + ".specificity"));
        }
        // probability is the chance of false positive or false negative
        int value = getDecision({probability});
        return value;
    }

    std::vector<double> Screening::getBackgroundScreeningProbability(
        std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];
        auto resultTable = table->selectWhere(selectCriteria);

        double prob = std::stod((*resultTable)["background_screening"][0]);
        std::vector<double> result = {prob, 1 - prob};
        return result;
    }

    std::vector<double> Screening::getInterventionScreeningProbability(
        std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];
        auto resultTable = table->selectWhere(selectCriteria);

        double prob = std::stod((*resultTable)["intervention_screening"][0]);
        std::vector<double> result = {prob, 1 - prob};
        return result;
    }
} // namespace Event

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
#include <string>

namespace event {
    void Screening::doEvent(person::PersonBase &person) {
        // one-time screen or periodic screen
        switch (this->interventionType) {
        case InterventionType::ONETIME:
            if (this->getCurrentTimestep() == 1) {
                this->interventionDecision(person);
            }
            break;
        case InterventionType::PERIODIC:
            // using braces to prevent strange scope for newly-declared
            // variables
            {
                int timeSinceLastScreening = this->getCurrentTimestep() -
                                             person->getTimeOfLastScreening();
                if (timeSinceLastScreening > this->interventionPeriod) {
                    this->interventionDecision(person);
                }
            }
            break;
        default:
            this->logger->error("Intervention Type not valid During Screening "
                                "Event with person {}",
                                person->getID());
            return;
        }

        // [screen, don't screen]
        std::vector<double> backgroundProbability =
            this->getBackgroundScreeningProbability(person);
        int choice = getDecision(backgroundProbability);
        if (choice == 0) {
            this->backgroundScreen(person);
        }
    }

    void Screening::interventionDecision(person::PersonBase &person) {
        std::vector<double> interventionProbability =
            this->getInterventionScreeningProbability(person);
        int choice = getDecision(interventionProbability);
        if (choice == 0) {
            this->interventionScreen(person);
        }
    }

    void Screening::backgroundScreen(person::PersonBase &person) {
        if (!(this->getCurrentTimestep() - person->getTimeOfLastScreening()) &&
            this->getCurrentTimestep() > 0) {
            return;
        }
        person->markScreened();

        std::string testPrefix = "screening_background_";
        bool firstTest = this->antibodyTest(person, testPrefix + "ab");
        this->insertScreeningCost(person, ScreeningType::BACKGROUND_AB);

        // if first test is negative, perform a second test
        bool secondTest = false;
        if (!firstTest) {
            secondTest = this->antibodyTest(person, testPrefix + "ab");
            this->insertScreeningCost(person, ScreeningType::BACKGROUND_AB);
        }

        if (!firstTest && !secondTest) {
            return; // run two tests and if both are negative do nothing
        }

        // if either is positive then...
        this->insertScreeningCost(person, ScreeningType::BACKGROUND_RNA);
        if (this->rnaTest(person, testPrefix + "rna")) {
            person->link(this->getCurrentTimestep(),
                         person::LinkageType::BACKGROUND);
            // what else needs to happen during a link?
        }

        person->unlink(this->getCurrentTimestep());
    }

    void Screening::interventionScreen(person::PersonBase &person) {
        person->markScreened();

        std::string testPrefix = "screening_intervention_";
        bool firstTest = this->antibodyTest(person, testPrefix + "ab");
        this->insertScreeningCost(person, ScreeningType::INTERVENTION_AB);

        // if first test is negative, perform a second test
        bool secondTest = false;
        if (!firstTest) {
            secondTest = this->antibodyTest(person, testPrefix + "ab");
        }

        this->insertScreeningCost(person, ScreeningType::INTERVENTION_RNA);
        if (this->rnaTest(person, testPrefix + "rna")) {
            person->link(this->getCurrentTimestep(),
                         person::LinkageType::INTERVENTION);
            // what else needs to happen during a link?
        } else {
            person->unlink(this->getCurrentTimestep());
        }
    }

    bool Screening::antibodyTest(person::PersonBase &person,
                                 std::string configKey) {
        double probability = 0.5;
        if (person->getSeropositive()) {
            person::HCV infectionStatus = person->getHCV();
            if (infectionStatus == person::HCV::ACUTE ||
                infectionStatus == person::HCV::NONE) {
                probability = 1 - std::get<double>(this->config.get(
                                      configKey + ".acute_sensitivity", 0.0));
            } else {
                probability = 1 - std::get<double>(this->config.get(
                                      configKey + ".chronic_sensitivity", 0.0));
            }
        } else {
            probability = std::get<double>(
                this->config.get(configKey + ".specificity", 0.0));
        }
        // probability is the chance of false positive or false negative
        int value = getDecision({probability});
        person->addAbScreen();
        return value;
    }

    bool Screening::rnaTest(person::PersonBase &person, std::string configKey) {
        double probability = 0.5;
        person::HCV infectionStatus = person->getHCV();
        if (infectionStatus == person::HCV::ACUTE) {
            probability = 1 - std::get<double>(this->config.get(
                                  configKey + ".acute_sensitivity", 0.0));
        } else if (infectionStatus == person::HCV::CHRONIC) {
            probability = 1 - std::get<double>(this->config.get(
                                  configKey + ".chronic_sensitivity", 0.0));
        } else {
            probability = std::get<double>(
                this->config.get(configKey + ".specificity", 0.0));
        }
        // probability is the chance of false positive or false negative
        int value = getDecision({probability});
        person->addRnaScreen();
        return value;
    }

    std::vector<double>
    Screening::getBackgroundScreeningProbability(person::PersonBase &person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            person::person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            person::person::behaviorEnumToStringMap[person->getBehavior()];
        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty()) {
            this->logger->error("No valid background screening probability "
                                "data for person {} characteristics.",
                                person->getID());
            return {};
        }

        double prob =
            std::stod((*resultTable)["background_screen_probability"][0]);

        std::string configStr = "";

        if (person->isBoomer()) {
            configStr = "screening.seropositive_multiplier_boomer";
        } else {
            configStr = "screening.seropositive_multiplier_not_boomer";
        }

        double multiplier;
        std::shared_ptr<Data::ReturnType> m =
            config.get_optional(configStr, 1.0);
        if (m) {
            multiplier = std::get<double>(*m);
        } else {
            multiplier = 1.0;
        }

        prob *= multiplier;
        std::vector<double> result = {prob, 1 - prob};
        return result;
    }

    std::vector<double>
    Screening::getInterventionScreeningProbability(person::PersonBase &person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)(person->age / 12.0));
        selectCriteria["gender"] =
            person::person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            person::person::behaviorEnumToStringMap[person->getBehavior()];
        auto resultTable = table->selectWhere(selectCriteria);
        if (resultTable->empty()) {
            // error
            return {};
        }

        double prob =
            std::stod((*resultTable)["intervention_screen_probability"][0]);
        std::vector<double> result = {prob, 1 - prob};
        return result;
    }

    void Screening::insertScreeningCost(person::PersonBase &person,
                                        ScreeningType type) {
        double screeningCost;
        std::string screeningName;
        switch (type) {
        case ScreeningType::BACKGROUND_AB:
            screeningCost = std::get<double>(
                this->config.get("screening_background_ab.cost", 0.0));
            screeningName = "Background Antibody Screening";
            break;
        case ScreeningType::BACKGROUND_RNA:
            screeningCost = std::get<double>(
                this->config.get("screening_background_rna.cost", 0.0));
            screeningName = "Background RNA Screening";
            break;
        case ScreeningType::INTERVENTION_AB:
            screeningCost = std::get<double>(
                this->config.get("screening_intervention_ab.cost", 0.0));
            screeningName = "Intervention Antibody Screening";
            break;
        case ScreeningType::INTERVENTION_RNA:
            screeningCost = std::get<double>(
                this->config.get("screening_intervention_rna.cost", 0.0));
            screeningName = "Intervention RNA Screening";
            break;
        }

        Cost::Cost cost = {this->costCategory, screeningName, screeningCost};
        person->addCost(cost, this->getCurrentTimestep());
    }
} // namespace event

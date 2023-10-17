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
    Screening::Screening(std::mt19937_64 &generator, Data::DataTable &table)
        : ProbEvent(generator, table){
              // QUERY backgroundProbability and interventionProbability Tables
              // Save to attributes
              // ensure lookup scheme for stratified age/IDU
          };

    void Screening::doEvent(std::shared_ptr<Person::Person> person) {
        double prob = 0.5;
        if ((person->isInterventionScreened() &&
             person->getScreeningFrequency() == -1 &&
             person->getTimeSinceLastScreening() == -1) ||
            (person->isInterventionScreened() &&
             person->getScreeningFrequency() != -1 &&
             person->getTimeSinceLastScreening() >
                 person->getScreeningFrequency())) {
            // time one-time screen or periodic screen
        } else {
            std::bernoulli_distribution backgroundProbability(prob);
            this->generatorMutex.lock();
            int value = backgroundProbability(this->generator);
            this->generatorMutex.unlock();

            if (value != 1) {
                return;
            }
            this->backgroundScreen(person);
        }
    }

    void Screening::backgroundScreen(std::shared_ptr<Person::Person> person) {
        if (person->getTimeSinceLastScreening() == 0 &&
            this->getCurrentTimestep() > 0) {
            return;
        }
        person->markScreened();

        if (!this->antibodyTest(person) && !this->antibodyTest(person)) {
            return; // run two tests and if both are negative do nothing
        }

        // if either is positive then...
        if (this->rnaTest(person)) {
            person->link(this->getCurrentTimestep(),
                         Person::LinkageType::BACKGROUND);
            // what else needs to happen during a link?
        }

        person->unlink(this->getCurrentTimestep());
    }

    void Screening::interventionScreen(std::shared_ptr<Person::Person> person) {
        if (person->getTimeSinceLastScreening() == 0 &&
            this->getCurrentTimestep() > 0) {
            return;
        }
        person->markScreened();
        std::bernoulli_distribution testAcceptanceProbability(
            this->acceptTestProbability[person->age]); // need to also add idu
                                                       // stratification
        this->generatorMutex.lock();
        int accepted = testAcceptanceProbability(this->generator);
        this->generatorMutex.unlock();
        if (!accepted) {
            return;
        }

        person->markScreened();
        if (!this->antibodyTest(person) && !this->antibodyTest(person)) {
        }
        if (this->rnaTest(person)) {
            person->link(this->getCurrentTimestep(),
                         Person::LinkageType::INTERVENTION);
            // what else needs to happen during a link?
        }
        person->unlink(this->getCurrentTimestep());
    }

    bool Screening::antibodyTest(std::shared_ptr<Person::Person> person) {
        double probability = 0.5;
        if (person->getSeropositivity()) {
            Person::HEPCState infectionStatus = person->getHEPCState();
            if (infectionStatus == Person::HEPCState::ACUTE ||
                infectionStatus == Person::HEPCState::NONE) {
                // probability = acute_sensitivity
            } else {
                // probability = chronic_sensitivity
            }
        } else {
            // probability = 1 - specificity;
        }
        std::bernoulli_distribution testProbability(probability);
        this->generatorMutex.lock();
        int value = testProbability(this->generator);
        this->generatorMutex.unlock();
        return value;
    }

    bool Screening::rnaTest(std::shared_ptr<Person::Person> person) {
        double probability = 0.5;
        Person::HEPCState infectionStatus = person->getHEPCState();
        if (infectionStatus == Person::HEPCState::ACUTE) {
            // probability = acute_sensitivity
        } else if (infectionStatus == Person::HEPCState::CHRONIC) {
            // probability = chronic_sensitivity
        } else {
            // probability = 1 - specificity;
        }
        std::bernoulli_distribution testProbability(probability);
        this->generatorMutex.lock();
        int value = testProbability(this->generator);
        this->generatorMutex.unlock();
        return value;
    }
} // namespace Event

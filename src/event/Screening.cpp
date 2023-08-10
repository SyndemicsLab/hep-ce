//===-- Screening.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid 
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Wednesday, August 9th 2023, 1:55:15 pm
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Screening.hpp"

namespace Event {
    Screening::Screening(uint64_t seed) {
        this->backgroundSeed = seed;
        this->generator.seed(this->backgroundSeed);
    };

    std::vector<Person::Person> Screening::execute(std::vector<Person::Person> population){
        std::for_each(std::execution::par, std::begin(population), std::end(population), this->screen);
    }

    void Screening::screen(Person::Person &person){
        if( person->isInterventionScreened() && 
            person->getScreeningFrequency() == -1 && 
            person->getTimeSinceLastScreening() == -1){
            // one time screen
        }
        else if(person->isInterventionScreened() && 
            person->getTimeSinceLastScreening() > person->getScreeningFrequency()){
            // time for periodic screen
        }
        else{
            std::bernoulli_distribution<int> backgroundProbability(1, prob);
            this->generatorMutex.lock();
            int value = backgroundProbability(this->generator);
            this->generatorMutex.unlock();
        }
    }
}
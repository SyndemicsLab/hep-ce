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
    Screening::Screening(std::mt19937_64 &generator) : generator(generator) {
        // QUERY backgroundProbability and interventionProbability Tables
        // Save to attributes
        // ensure lookup scheme for stratified age/IDU
    };

    void Screening::execute(std::vector<Person::Person> &population, int timestep){
        this->currentTimestep = timestep;
        std::for_each(std::execution::par, std::begin(population), std::end(population), screen);
    }

    void Screening::screen(Person::Person &person){
        double prob = 0.5;
        if( 
            (person.isInterventionScreened() && 
                person.getScreeningFrequency() == -1 && 
                person.getTimeSinceLastScreening() == -1) ||
            (person.isInterventionScreened() && 
                person.getScreeningFrequency() != -1 && 
                person.getTimeSinceLastScreening() > person.getScreeningFrequency())
        ){
            // time one-time screen or periodic screen
        }
        else{
            std::bernoulli_distribution backgroundProbability(prob);
            this->generatorMutex.lock();
            int value = backgroundProbability(this->generator);
            this->generatorMutex.unlock();
            
            if (value != 1){
                return;
            }
            this->backgroundScreen(person);

        }
    }

    void Screening::backgroundScreen(Person::Person &person){
        person.markScreened();

        if(!this->antibodyTest(person) && !this->antibodyTest(person)){
            return; // run two tests and if both are negative do nothing
        }
        
        // if either is positive then...
        if(this->rnaTest(person)){
            person.link(this->currentTimestep, Person::LinkageType::BACKGROUND);
            // what else needs to happen during a link?
        }

        person.unlink(this->currentTimestep);
    }

    /// @brief 
    /// @param person 
    void Screening::interventionScreen(Person::Person &person){
        std::bernoulli_distribution testAcceptanceProbability(this->acceptTestProbability[person.age]); // need to also add idu stratification
        this->generatorMutex.lock();
        int accepted = testAcceptanceProbability(this->generator);
        this->generatorMutex.unlock();
        if(!accepted) {
            return;
        }

        person.markScreened();
        if(!this->antibodyTest(person) && !this->antibodyTest(person)){

        }
        if(this->rnaTest(person)){
            person.link(this->currentTimestep, Person::LinkageType::INTERVENTION);
            // what else needs to happen during a link?
        }
        person.unlink(this->currentTimestep);
    }

    /// @brief 
    /// @param person 
    /// @return 
    bool Screening::antibodyTest(Person::Person &person){
        double probability = 0.5;
        if(person.getSeropositivity()){
            Person::HEPCState infectionStatus = person.getHEPCState();
            if(infectionStatus == Person::HEPCState::ACUTE || infectionStatus == Person::HEPCState::NONE){
                // probability = acute_sensitivity
            }   
            else{
                // probability = chronic_sensitivity
            }
        }
        else{
            // probability = 1 - specificity;
        }
        std::bernoulli_distribution testProbability(probability);
        this->generatorMutex.lock();
        int value = testProbability(this->generator);
        this->generatorMutex.unlock();
        return value;
    }

    bool Screening::rnaTest(Person::Person &person){
        double probability = 0.5;
        Person::HEPCState infectionStatus = person.getHEPCState();
        if(infectionStatus == Person::HEPCState::ACUTE){
            // probability = acute_sensitivity
        }   
        else if(infectionStatus == Person::HEPCState::CHRONIC){
            // probability = chronic_sensitivity
        }
        else{
            // probability = 1 - specificity;
        }
        std::bernoulli_distribution testProbability(probability);
        this->generatorMutex.lock();
        int value = testProbability(this->generator);
        this->generatorMutex.unlock();
        return value;
    }
}
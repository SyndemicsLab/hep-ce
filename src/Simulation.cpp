//===-- Simulation.cpp - Instruction class definition -------*- C++ -*-===//
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
/// Created Date: Wednesday, August 2nd 2023, 10:01:54 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Simulation.hpp"

// PRIVATE METHODS

std::vector<Person> Simulation::createPopulation(std::mt19937_64 generator) {
    return std::vector<Person>();
}

std::vector<Event> Simulation::createEvents(std::mt19937_64 generator) {
    return std::vector<Event>();
}

// PUBLIC METHODS

std::vector<Person> Simulation::createPopulation() {
    this->loadPopulation(this->createPopulation(this->generator));
    return this->getPopulation();
}

std::vector<Event> Simulation::createEvents() { 
    this->loadEvents(this->createEvents(this->generator)); 
    return this->getEvents();
}

void Simulation::loadPopulation(std::vector<Person> population) {}

void Simulation::addPerson(Person person) {}

void Simulation::loadEvents(std::vector<Event> events) {
    if (events.empty()) {
        // log that we are writing an empty event vector
    }
    if (!this->events.empty()){
        // log that we are overwriting this->events
    }
    this->events = events;
}

bool Simulation::addEventToEnd(Event event) { return false; }

bool Simulation::addEventToBeginning(Event event) { return false; }

bool Simulation::addEventAtIndex(Event event, int idx) { return false; }

std::vector<Person> Simulation::getPopulation() { return this->population; }

std::vector<Event> Simulation::getEvents() { return this->events; }

std::vector<Person> Simulation::run(){ 
    while(this->currentTimestep < this->duration){
        for(Event event : this->events){
            this->population = event.execute(this->population);
        }
    }
    return this->population;
}

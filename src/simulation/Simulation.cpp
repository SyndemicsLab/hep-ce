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

// PUBLIC METHODS

void Simulation::createPopulation(const int N) {
    for (int i = 0; i < N; ++i) {
        this->population.push_back(std::make_shared<Person::Person>());
    }
}

std::vector<std::shared_ptr<Event::Event>> Simulation::createEvents() {
    this->loadEvents(std::vector<std::shared_ptr<Event::Event>>());
    return this->getEvents();
}

void Simulation::loadPopulation(std::vector<std::shared_ptr<Person::Person>> &population) {
    this->population = population;
}

void Simulation::addPerson(Person::Person person) {}

void Simulation::loadEvents(std::vector<std::shared_ptr<Event::Event>> events) {
    if (events.empty()) {
        // log that we are writing an empty event vector
    }
    if (!this->events.empty()) {
        // log that we are overwriting this->events
    }
    this->events = events;
}

bool Simulation::addEventToEnd(Event::Event &event) { return false; }

bool Simulation::addEventToBeginning(Event::Event &event) { return false; }

bool Simulation::addEventAtIndex(Event::Event &event, int idx) { return false; }

std::vector<std::shared_ptr<Person::Person>> Simulation::getPopulation() {
    return this->population;
}

std::vector<std::shared_ptr<Event::Event>> Simulation::getEvents() {
    return this->events;
}

std::vector<std::shared_ptr<Person::Person>> Simulation::run() {
    while (this->currentTimestep < this->duration) {
        for (std::shared_ptr<Event::Event> event : this->events) {
            event->execute(this->population, this->currentTimestep);
        }
        this->currentTimestep++;
    }
    return this->population;
}

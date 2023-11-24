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
/// This file contains the implementation of the Simulation Class.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Simulation.hpp"

namespace Simulation {
    std::mt19937_64 Simulation::generator;
    void Simulation::createPopulation(const int N) {
        for (int i = 0; i < N; ++i) {
            this->population.push_back(std::make_shared<Person::Person>());
        }
    }

    std::vector<std::shared_ptr<Event::Event>> Simulation::createEvents() {
        this->loadEvents(std::vector<std::shared_ptr<Event::Event>>());
        return this->getEvents();
    }

    void Simulation::loadPopulation(
        std::vector<std::shared_ptr<Person::Person>> &population) {
        if (!this->population.empty()) {
            // log that we are over-writing the population
        }
        this->population = population;
    }

    void Simulation::addPerson(Person::Person person) {
        this->population.push_back(std::make_shared<Person::Person>(person));
    }

    void
    Simulation::loadEvents(std::vector<std::shared_ptr<Event::Event>> events) {
        if (events.empty()) {
            // log that we are writing an empty event vector
        }
        if (!this->events.empty()) {
            // log that we are overwriting this->events
        }
        this->events = events;
    }

    void Simulation::addEventToEnd(std::shared_ptr<Event::Event> event) {
        // log that an event is being added to the end of the event list
        this->events.push_back(event);
    }

    void Simulation::addEventToBeginning(std::shared_ptr<Event::Event> event) {
        // log that an event is being inserted at the beginning of the event
        // list
        this->events.insert(this->events.begin(), event);
    }

    bool Simulation::addEventAtIndex(std::shared_ptr<Event::Event> event,
                                     int idx) {
        if (idx >= this->events.size() || idx < 0) {
            // index out of range
            return false;
        }
        // log that an event is being added to the event list in index idx
        this->events.insert(this->events.begin() + idx, event);
        return true;
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
} // namespace Simulation
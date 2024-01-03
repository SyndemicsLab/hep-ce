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
        if (N > 0) {
            this->population = {};
        }
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
            this->logger->info("Simulation Population not empty, overwriting "
                               "existing population.");
        }
        this->population = population;
    }

    void Simulation::addPerson(Person::Person person) {
        this->population.push_back(std::make_shared<Person::Person>(person));
    }

    void
    Simulation::loadEvents(std::vector<std::shared_ptr<Event::Event>> events) {
        if (events.empty()) {
            this->logger->warn("Empty Event being added to simulation event "
                               "list! Make sure this is intentional.");
        }
        if (!this->events.empty()) {
            this->logger->info(
                "Simulation Events not empty, overwriting existing events.");
        }
        this->events = events;
    }

    void Simulation::addEventToEnd(std::shared_ptr<Event::Event> event) {
        this->logger->info("Event being added to end of Event Queue");
        this->events.push_back(event);
    }

    void Simulation::addEventToBeginning(std::shared_ptr<Event::Event> event) {
        this->logger->info("Event being added to beginning of Event Queue.");
        this->events.insert(this->events.begin(), event);
    }

    bool Simulation::addEventAtIndex(std::shared_ptr<Event::Event> event,
                                     int idx) {
        if (idx >= this->events.size() || idx < 0) {
            this->logger->warn(
                "Index {0} out of Event Queue Range of size {1}!",
                std::to_string(idx), std::to_string(this->events.size()));
            return false;
        }
        this->logger->info("Event being added to index {0} of Event Queue.",
                           std::to_string(idx));
        this->events.insert(this->events.begin() + idx, event);
        return true;
    }

    std::vector<std::shared_ptr<Person::Person>> Simulation::run() {
        this->logger->info("Simulation Run Started");
        for (uint32_t timestep = this->currentTimestep;
             timestep < ((int)this->duration); ++timestep) {
            for (std::shared_ptr<Event::Event> event : this->events) {
                event->setCurrentTimestep();
                event->execute(this->population);
            }
            this->logger->info("Simulation completed timestep {}", timestep);
        }
        return this->population;
    }
} // namespace Simulation
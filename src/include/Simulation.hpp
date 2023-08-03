//===-- Simulation.hpp - Instruction class definition -------*- C++ -*-===//
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
/// Created Date: Wednesday, August 2nd 2023, 9:36:31 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include "Person.hpp"
#include "Event.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

class Simulation {
private:
    uint32_t currentTimestep = 0;
    uint64_t seed;
    std::vector<Person::Person> population;
    std::vector<Event::Event> events;
    std::mt19937_64 generator;
    
    std::vector<Person::Person> createPopulation(std::mt19937_64 generator);
    std::vector<Event::Event> createEvents(std::mt19937_64 generator);

public:
    Simulation() : Simulation((uint64_t)0, (uint32_t)0) {};

    Simulation(uint64_t seed) : Simulation(seed, (uint32_t)0) {};

    Simulation(uint32_t duration) : Simulation((uint64_t)0, duration) {};

    Simulation(uint64_t seed, uint32_t duration){
        this->generator.seed(this->seed);
        this->duration = duration;
    }

    virtual ~Simulation() = default;

    uint32_t duration;

    std::vector<Person::Person> createPopulation();
    std::vector<Event::Event> createEvents();

    void loadPopulation(std::vector<Person::Person> population);
    void addPerson(Person::Alive person);

    void loadEvents(std::vector<Event::Event> events);
    bool addEventToEnd(Event::Event event);
    bool addEventToBeginning(Event::Event event);
    bool addEventAtIndex(Event::Event event, int idx);

    std::vector<Person::Person> getPopulation();
    std::vector<Event::Event> getEvents();

    std::vector<Person::Person> run();

};

#endif
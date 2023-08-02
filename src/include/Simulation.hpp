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
#include <string>
#include <vector>

class Simulation {
private:
    uint32_t currentTimestep;
    unsigned long long seed;
    std::vector<Person> population;
    std::vector<Event> events;

public:
    Simulation() : seed(0) {};
    Simulation(unsigned long long seed) : seed(seed) {};

    Simulation(uint32_t duration) : seed(0) {
        this->duration = duration;
    };

    virtual ~Simulation() = default;

    uint32_t duration;

    void loadPopulation(std::vector<Person> population);
    void addPerson(Person person);

    void loadEvents(std::vector<Event> events);
    bool addEventToEnd(Event event);
    bool addEventToBeginning(Event event);
    bool addEventAtIndex(Event event, int idx);

    std::vector<Person> getPopulation();
    std::vector<Event> getEvents();

    std::vector<Person> run();
    std::vector<Person> run(uint32_t duration);

};

#endif
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

#include "Event.hpp"
#include "Person.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

/// @brief
class Simulation {
private:
    uint32_t currentTimestep = 0;
    uint64_t seed;
    std::vector<Person::Person> population;
    std::vector<std::shared_ptr<Event::Event>> events;
    std::mt19937_64 generator;

public:
    Simulation() : Simulation((uint64_t)0, (uint32_t)0){};

    Simulation(uint64_t seed) : Simulation(seed, (uint32_t)0){};

    Simulation(uint32_t duration) : Simulation((uint64_t)0, duration){};

    Simulation(uint64_t seed, uint32_t duration) {
        this->generator.seed(this->seed);
        this->duration = duration;
    }

    virtual ~Simulation() = default;

    uint32_t duration;

    /// @brief
    /// @return
    std::vector<Person::Person> createPopulation();

    /// @brief
    /// @return
    std::vector<std::shared_ptr<Event::Event>> createEvents();

    /// @brief
    /// @param population
    void loadPopulation(std::vector<Person::Person> population);

    /// @brief
    /// @param person
    void addPerson(Person::Person person);

    /// @brief
    /// @param events
    void loadEvents(std::vector<std::shared_ptr<Event::Event>> events);

    /// @brief
    /// @param event
    /// @return
    bool addEventToEnd(Event::Event &event);

    /// @brief
    /// @param event
    /// @return
    bool addEventToBeginning(Event::Event &event);

    /// @brief
    /// @param event
    /// @param idx
    /// @return
    bool addEventAtIndex(Event::Event &event, int idx);

    /// @brief
    /// @return
    std::vector<Person::Person> getPopulation();

    /// @brief
    /// @return
    std::vector<std::shared_ptr<Event::Event>> getEvents();

    /// @brief
    /// @return
    std::vector<Person::Person> run();

    /// @brief Access the random number generator, for events that need to
    /// sample the pRNG
    /// @return Reference to the simulation's pseudorandom number generator
    std::mt19937_64 &getGenerator() { return generator; }
};

#endif

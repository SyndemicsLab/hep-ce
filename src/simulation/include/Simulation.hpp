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
/// This file contains the declaration of the Simulation Class.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include "Event.hpp"

#include <memory>

/// @brief Namespace containing Simulation Level Attributes
namespace Simulation {
    class Simulation {
    private:
        uint32_t currentTimestep = 0;
        uint64_t seed;
        std::vector<std::shared_ptr<Person::Person>> population;
        std::vector<std::shared_ptr<Event::Event>> events;
        static std::mt19937_64 generator;

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

        /// @brief Function used to Create Population Set
        /// @param N Number of People to create in the Population
        void createPopulation(const int N);

        /// @brief Function used to Create Events List
        /// @return Vector of Events to be used in the Simulation
        std::vector<std::shared_ptr<Event::Event>> createEvents();

        /// @brief Function to Load the Population to the Simulation
        /// @param population Population Set Loaded to Simulation
        void loadPopulation(
            std::vector<std::shared_ptr<Person::Person>> &population);

        /// @brief Function used to add a Single Person to the Population in the
        /// Simulation
        /// @param person Person to add to the Population
        void addPerson(Person::Person person);

        /// @brief Function used to load the Events List to the Simulation
        /// @param events Events List to Load to the Simulation
        void loadEvents(std::vector<std::shared_ptr<Event::Event>> events);

        /// @brief Add an Event to the end of the Event List
        /// @param event Event to add to the Simulation Event List
        void addEventToEnd(std::shared_ptr<Event::Event> event);

        /// @brief Add an Event to the beginning of the Event List
        /// @param event Event to add to the Simulation Event List
        void addEventToBeginning(std::shared_ptr<Event::Event> event);

        /// @brief Add an Event to the provided index in the Event List
        /// @param event Even to add to the Simulation Event List
        /// @param idx Index of the location to add the Event
        /// @return True if it succeeds, False if it fails
        bool addEventAtIndex(std::shared_ptr<Event::Event> event, int idx);

        /// @brief Retrieve the Population Vector from the Simulation
        /// @return List of People in the Simulation
        std::vector<std::shared_ptr<Person::Person>> getPopulation() const {
            return this->population;
        }

        /// @brief Retrieve the Events in the Simulation
        /// @return List of Events in the Simulation
        std::vector<std::shared_ptr<Event::Event>> getEvents() const {
            return this->events;
        }

        /// @brief Execute the Simulation
        /// @return The Final State of the entire Population
        std::vector<std::shared_ptr<Person::Person>> run();

        /// @brief Access the random number generator, for events that need to
        /// sample the pRNG
        /// @return Reference to the simulation's pseudorandom number generator
        std::mt19937_64 &getGenerator() { return generator; }
    };

} // namespace Simulation
#endif

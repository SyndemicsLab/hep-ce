//===-- Event.hpp - Instruction class definition -------*- C++ -*-===//
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
/// Created Date: Wednesday, August 2nd 2023, 9:44:04 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include "Person.hpp"
#include <algorithm>
#include <execution>
#include <mutex>
#include <random>
#include <vector>

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {
    /// @brief Abstract class that superclasses all Events. Contains execute
    /// function definition
    class Event {
    private:
        int currentTimestep = -1;
        virtual void doEvent(Person::Person &person) = 0;

    public:
        Event(){};
        virtual ~Event() = default;

        int getCurrentTimestep() { return this->currentTimestep; }

        /// @brief Primary function required in all Event subclasses. Used to
        /// run the event
        /// @param  Population vector containing all the people in the
        /// simulation
        /// @param  timestep integer containing the current timestep of the
        /// simulation
        /// @return The population vector after the event is executed
        void execute(std::vector<Person::Person> &population, int timestep) {
            this->currentTimestep = timestep;
            std::for_each(std::execution::par, std::begin(population),
                          std::end(population),
                          [this](Person::Person &p) { this->doEvent(p); });
        }
    };

    /// @brief Abstract class for use with Events that involve sampling from the
    /// random number generator to make decisions.
    class ProbEvent : public Event {
    protected:
        std::mt19937_64 &generator;
        std::mutex generatorMutex;

        /// @brief When making a decision with two or more choices, pick one
        /// based on the provided weight(s).
        /// @param probs A vector containing the weights of each option.
        /// @return
        int getDecision(std::vector<double> probs) {
            if (std::accumulate(probs.begin(), probs.end(), 0.0) > 1.0) {
                // error -- sum of probabilities cannot exceed 1
                return -1;
            }
            std::uniform_real_distribution<double> uniform(0.0, 1.0);
            this->generatorMutex.lock();
            double value = uniform(this->generator);
            this->generatorMutex.unlock();
            double reference = 0.0;
            for (int i = 0; i < probs.size(); ++i) {
                reference += probs[i];
                if (value < reference) {
                    return i;
                }
            }
            return (int)probs.size();
        }
    public:
        ProbEvent(std::mt19937_64 &generator): generator(generator) {}
        virtual ~ProbEvent() = default;
    };
} // namespace Event
#endif

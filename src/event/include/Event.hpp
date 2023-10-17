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
/// This file contains the declaration of the Event and ProbEvent Abstract
/// Classes.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include "Person.hpp"
#include "SQLite3.hpp"
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
        virtual void doEvent(std::shared_ptr<Person::Person> person) = 0;

    public:
        Event(){};
        virtual ~Event() = default;

        int getCurrentTimestep() const { return this->currentTimestep; }

        /// @brief Primary function required in all Event subclasses. Used to
        /// run the event
        /// @param  Population vector containing all the people in the
        /// simulation
        /// @param  timestep integer containing the current timestep of the
        /// simulation
        /// @return The population vector after the event is executed
        void execute(std::vector<std::shared_ptr<Person::Person>> &population,
                     int timestep) {
            this->currentTimestep = timestep;
            std::for_each(std::execution::par, std::begin(population),
                          std::end(population),
                          [this](std::shared_ptr<Person::Person> &p) {
                              this->doEvent(p);
                          });
        }
    };

    /// @brief Abstract class for use with Events that involve sampling from the
    /// random number generator to make decisions.
    class ProbEvent : public Event {
    protected:
        std::mt19937_64 &generator;
        Data::Database &db;
        std::mutex generatorMutex;

        /// @brief When making a decision with two or more choices, pick one
        /// based on the provided weight(s).
        /// @details The weights specified in the argument \code{probs} must sum
        /// to no greater than 1.0. In the case that probabilities sum to 1,
        /// the return value is always an index of the vector (i.e. the return
        /// is always < \code{probs.size()}). If the sum of the probabilities is
        /// less than 1.0, then the difference (\code{1.0 - sum(probs)}) is
        /// the probability to draw return value \code{probs.size()}.
        /// @param probs A vector containing the weights of each option.
        /// @return Integer representing the chosen state.
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
        ProbEvent(std::mt19937_64 &generator, Data::Database &database)
            : generator(generator), db(database) {}
        virtual ~ProbEvent() = default;
    };
} // namespace Event
#endif

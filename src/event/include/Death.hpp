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
/// This file contains the declaration of the Death Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_DEATH_HPP_
#define EVENT_DEATH_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {

    /// @brief Subclass of Event used to End the Aging Process of Individuals
    class Death : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;

        /// @brief The actual death of a person
        /// @param person Person who dies
        void die(std::shared_ptr<Person::Person> person);

        void
        getMortalityProbabilities(std::shared_ptr<Person::Person> const person,
                                  double &fatalOverdoseProb,
                                  double &backgroundMortProb, double &smr,
                                  double &fibrosisDeathProb);

    public:
        Death(std::mt19937_64 &generator, Data::IDataTablePtr table,
              Data::Configuration &config,
              std::shared_ptr<spdlog::logger> logger =
                  std::make_shared<spdlog::logger>("default"),
              std::string name = std::string("Death"))
            : ProbEvent(generator, table, config, logger, name) {}
        virtual ~Death() = default;
    };
} // namespace Event

#endif

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
/// This file contains the declaration of the Infections Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_INFECTIONS_HPP_
#define EVENT_INFECTIONS_HPP_

#include <cmath>

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to Spread Infections
    class Infections : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;

        /// @brief Retrieve Infection Probabilities for Infection Chances for
        /// the individual Person from the SQL Table
        /// @param person Person whom to retrieve Infection Probabilities for
        /// @return Vector of Infection Probabilities
        std::vector<double> getInfectProb(person::PersonBase &person);

    public:
        Infections(std::mt19937_64 &generator, Data::IDataTablePtr table,
                   Data::Config &config,
                   std::shared_ptr<spdlog::logger> logger =
                       std::make_shared<spdlog::logger>("default"),
                   std::string name = std::string("Infections"))
            : ProbEvent(generator, table, config, logger, name) {}
        virtual ~Infections() = default;
    };
} // namespace event

#endif

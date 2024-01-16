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
/// This file contains the declaration of the Clearance Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_CLEARANCE_HPP_
#define EVENT_CLEARANCE_HPP_
#include "Event.hpp"
#include "Utils.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {
    /// @brief Subclass of Event used to Clear HCV Infections
    class Clearance : public ProbEvent {
    private:
        // probabilityToRate doesn't include time, hence division by 6.0
        double clearanceProb = Utils::probabilityToRate(0.25) / 6.0;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;
        /// @brief Returns the probability of acute clearance
        /// @details Typically, there's a 25% chance of acute hcv clearance in
        /// the first six months of infection.
        std::vector<double> getClearanceProb();

    public:
        Clearance(std::mt19937_64 &generator, Data::IDataTablePtr table,
                  Data::Configuration &config,
                  std::shared_ptr<spdlog::logger> logger =
                      std::make_shared<spdlog::logger>("default"),
                  std::string name = std::string("ProbEvent"))
            : ProbEvent(generator, table, config, logger, name) {}
        virtual ~Clearance() = default;
    };
} // namespace Event
#endif // EVENT_CLEARANCE_HPP_

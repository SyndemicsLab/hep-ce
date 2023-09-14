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
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;
        std::vector<double> getClearanceProb();

    public:
        using ProbEvent::ProbEvent;
        virtual ~Clearance() = default;
    };
} // namespace Event
#endif // EVENT_CLEARANCE_HPP_

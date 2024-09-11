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
#include "Decider.hpp"
#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    /// @brief Subclass of Event used to Clear HCV Infections
    class Clearance : public Event {
    private:
        class ClearanceIMPL;
        std::unique_ptr<ClearanceIMPL> impl;
        std::shared_ptr<stats::Decider> decider;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;

    public:
        Clearance(std::shared_ptr<stats::Decider> decider,
                  std::shared_ptr<datamanagement::DataManager> dm,
                  std::string name = std::string("Clearance"));
        virtual ~Clearance() = default;
    };
} // namespace event
#endif // EVENT_CLEARANCE_HPP_

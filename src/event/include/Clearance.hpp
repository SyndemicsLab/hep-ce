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
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    /// @brief Subclass of Event used to Clear HCV Infections
    class Clearance : public Event {
    private:
        class ClearanceIMPL;
        std::unique_ptr<ClearanceIMPL> impl;

        /// @brief Implementation of Virtual Function DoEvent
        /// @param person Individual Person undergoing Event
        void DoEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) override;

    public:
        Clearance(std::shared_ptr<datamanagement::DataManagerBase> dm);
        ~Clearance();

        // Copy Operations
        Clearance(Clearance const &) = delete;
        Clearance &operator=(Clearance const &) = delete;
        Clearance(Clearance &&) noexcept;
        Clearance &operator=(Clearance &&) noexcept;
    };
} // namespace event
#endif // EVENT_CLEARANCE_HPP_

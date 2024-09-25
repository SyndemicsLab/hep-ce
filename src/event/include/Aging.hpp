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
/// This file contains the declaration of the Aging Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_AGING_HPP_
#define EVENT_AGING_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    /// @brief Subclass of Event used to Increase the Age of a Person
    class Aging : public Event {
    private:
        class AgingIMPL;
        std::unique_ptr<AgingIMPL> impl;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) override;

    public:
        Aging();
        ~Aging();

        // Copy Operations
        Aging(Aging const &) = delete;
        Aging &operator=(Aging const &) = delete;
        Aging(Aging &&) noexcept;
        Aging &operator=(Aging &&) noexcept;
    };
} // namespace event
#endif

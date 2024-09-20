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
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to End the Death Process of Individuals
    class Death : public Event {
    private:
        class DeathIMPL;
        std::unique_ptr<DeathIMPL> impl;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) override;

    public:
        Death();
        ~Death();

        // Copy Operations
        Death(Death const &) = delete;
        Death &operator=(Death const &) = delete;
        Death(Death &&) noexcept;
        Death &operator=(Death &&) noexcept;
    };
} // namespace event

#endif
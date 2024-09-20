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

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to Spread Infections
    class Infections : public Event {
    private:
        class InfectionsIMPL;
        std::unique_ptr<InfectionsIMPL> impl;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::Decider> decider) override;

    public:
        Infections();
        ~Infections();

        // Copy Operations
        Infections(Infections const &) = delete;
        Infections &operator=(Infections const &) = delete;
        Infections(Infections &&) noexcept;
        Infections &operator=(Infections &&) noexcept;
    };
} // namespace event

#endif

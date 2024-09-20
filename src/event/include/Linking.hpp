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
/// This file contains the declaration of the Linking Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_Linking_HPP_
#define EVENT_Linking_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to Link People to Treatment
    class Linking : public Event {
    private:
        class LinkingIMPL;
        std::unique_ptr<LinkingIMPL> impl;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::Decider> decider) override;

    public:
        Linking();
        ~Linking();

        // Copy Operations
        Linking(Linking const &) = delete;
        Linking &operator=(Linking const &) = delete;
        Linking(Linking &&) noexcept;
        Linking &operator=(Linking &&) noexcept;
    };
} // namespace event
#endif

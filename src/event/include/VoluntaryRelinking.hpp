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
/// This file contains the declaration of the VoluntaryRelinking Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_VOLUNTARYRELINK_HPP_
#define EVENT_VOLUNTARYRELINK_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to Create Voluntary Relinkage to Treatment
    /// for People
    class VoluntaryRelinking : public Event {
    private:
        class VoluntaryRelinkingIMPL;
        std::unique_ptr<VoluntaryRelinkingIMPL> impl;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::unique_ptr<stats::Decider> &decider) override;

    public:
        VoluntaryRelinking();
        ~VoluntaryRelinking();

        // Copy Operations
        VoluntaryRelinking(VoluntaryRelinking const &) = delete;
        VoluntaryRelinking &operator=(VoluntaryRelinking const &) = delete;
        VoluntaryRelinking(VoluntaryRelinking &&) noexcept;
        VoluntaryRelinking &operator=(VoluntaryRelinking &&) noexcept;
    };

} // namespace event

#endif

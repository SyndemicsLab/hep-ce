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
#include "Person.hpp"
#include "sqlite3.h"

#include <mutex>
#include <random>

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {

    /// @brief Subclass of Event used to Create Voluntary Relinkage to Treatment
    /// for People
    class VoluntaryRelinking : public ProbEvent {
    private:
        /// @brief Parameter used to set the Total Potential Relink Time
        int voluntaryRelinkDuration = 1024; // we should set this

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        using ProbEvent::ProbEvent;
        virtual ~VoluntaryRelinking() = default;

        void setVoluntaryRelinkDuration(int duration) {
            this->voluntaryRelinkDuration = duration;
        }

        int getVoluntaryRelinkDuration() {
            return this->voluntaryRelinkDuration;
        }
    };

} // namespace Event

#endif

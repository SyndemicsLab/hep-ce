//===-- VoluntaryRelinking.hpp - Instruction class definition -------*- C++
//-*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
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

namespace Event {

    class VoluntaryRelinking : public ProbEvent {
    private:
        /* data */
        int voluntaryRelinkDuration = 1024; // we should set this

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

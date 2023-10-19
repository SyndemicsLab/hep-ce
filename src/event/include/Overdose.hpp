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
/// This file contains the declaration of the Overdose Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef EVENT_OVERDOSE_HPP_
#define EVENT_OVERDOSE_HPP_

#include "Event.hpp"

namespace Event {
    /// @brief Subclass of Event used to simulate overdoses among active drug
    /// users.
    class Overdose : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;

        /// @brief Get the probability that a person will overdose.
        /// @param person Pointer to the relevant Person::Person object which
        /// might overdose.
        /// @return Overdose probability based on person attributes
        double getProbability(std::shared_ptr<Person::Person> person);

    public:
        using ProbEvent::ProbEvent;
        virtual ~Overdose() = default;
    };
} // namespace Event
#endif // EVENT_OVERDOSE_HPP_

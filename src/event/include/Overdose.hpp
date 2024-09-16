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

namespace event {
    /// @brief Subclass of Event used to simulate overdoses among active drug
    /// users.
    class Overdose : public Event {
    private:
        class OverdoseIMPL;
        std::shared_ptr<OverdoseIMPL> impl;
        std::shared_ptr<stats::Decider> decider;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual person undergoing Event
        void doEvent(person::PersonBase &person) override;

    public:
        Overdose(std::shared_ptr<stats::Decider> decider,
                 std::shared_ptr<datamanagement::DataManager> dm,
                 std::string name = std::string("Overdose"));
        virtual ~Overdose() = default;
    };
} // namespace event
#endif // EVENT_OVERDOSE_HPP_

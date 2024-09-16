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
/// This file contains the declaration of the BehaviorChanges Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_BEHAVIORCHANGES_HPP_
#define EVENT_BEHAVIORCHANGES_HPP_

#include "Event.hpp"

namespace event {
    /// @brief Subclass of Event used to process Behavior Changes
    class BehaviorChanges : public Event {
    private:
        class BehaviorChangesIMPL;
        std::shared_ptr<BehaviorChangesIMPL> impl;
        std::shared_ptr<stats::Decider> decider;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;

    public:
        BehaviorChanges(std::shared_ptr<stats::Decider> decider,
                        std::shared_ptr<datamanagement::DataManager> dm,
                        std::string name = std::string("BehaviorChanges"));
        virtual ~BehaviorChanges() = default;
    };
} // namespace event
#endif // EVENT_BEHAVIORCHANGES_HPP_

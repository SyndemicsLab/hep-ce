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
        std::unique_ptr<BehaviorChangesIMPL> impl;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) override;

    public:
        BehaviorChanges();
        ~BehaviorChanges();

        // Copy Operations
        BehaviorChanges(BehaviorChanges const &) = delete;
        BehaviorChanges &operator=(BehaviorChanges const &) = delete;
        BehaviorChanges(BehaviorChanges &&) noexcept;
        BehaviorChanges &operator=(BehaviorChanges &&) noexcept;
    };
} // namespace event
#endif // EVENT_BEHAVIORCHANGES_HPP_

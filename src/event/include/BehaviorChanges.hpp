////////////////////////////////////////////////////////////////////////////////
// File: BehaviorChanges.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-09-13                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_BEHAVIORCHANGES_HPP_
#define EVENT_BEHAVIORCHANGES_HPP_

#include "Event.hpp"

namespace event {
/// @brief Subclass of Event used to process Behavior Changes
class BehaviorChanges : public Event {
private:
    class BehaviorChangesIMPL;
    std::unique_ptr<BehaviorChangesIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    BehaviorChanges(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~BehaviorChanges();

    // Copy Operations
    BehaviorChanges(BehaviorChanges const &) = delete;
    BehaviorChanges &operator=(BehaviorChanges const &) = delete;
    BehaviorChanges(BehaviorChanges &&) noexcept;
    BehaviorChanges &operator=(BehaviorChanges &&) noexcept;
};
} // namespace event
#endif // EVENT_BEHAVIORCHANGES_HPP_

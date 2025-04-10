////////////////////////////////////////////////////////////////////////////////
// File: HIVLinking.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-25                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-09                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HIVLINKING_HPP_
#define EVENT_HIVLINKING_HPP_

#include "Event.hpp"

namespace event {
class HIVLinkingIMPL;

class HIVLinking : public Event {
private:
    std::unique_ptr<HIVLinkingIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    /// @param dm DataManager object containing data for the event
    /// @param decider Pseudorandom number generator for decision-making
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HIVLinking(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~HIVLinking();

    // Copy Operations
    HIVLinking(HIVLinking const &) = delete;
    HIVLinking &operator=(HIVLinking const &) = delete;
    HIVLinking(HIVLinking &&) noexcept;
    HIVLinking &operator=(HIVLinking &&) noexcept;
};
} // namespace event
#endif

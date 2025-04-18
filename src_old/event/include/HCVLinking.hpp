////////////////////////////////////////////////////////////////////////////////
// File: HCVLinking.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-11                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HCVLINKING_HPP_
#define EVENT_HCVLINKING_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
class HCVLinkingIMPL;

/// @brief Subclass of Event used to Link People to Treatment
class HCVLinking : public Event {
private:
    std::unique_ptr<HCVLinkingIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    /// @param dm DataManager object containing data for the event
    /// @param decider Pseudorandom number generator for decision-making
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HCVLinking(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~HCVLinking();

    // Copy Operations
    HCVLinking(HCVLinking const &) = delete;
    HCVLinking &operator=(HCVLinking const &) = delete;
    HCVLinking(HCVLinking &&) noexcept;
    HCVLinking &operator=(HCVLinking &&) noexcept;
};
} // namespace event
#endif

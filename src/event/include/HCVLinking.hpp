////////////////////////////////////////////////////////////////////////////////
// File: HCVLinking.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-19                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HCVLINKING_HPP_
#define EVENT_HCVLINKING_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

/// @brief Subclass of Event used to Link People to Treatment
class HCVLinking : public Event {
private:
    class HCVLinkingIMPL;
    std::unique_ptr<HCVLinkingIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
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

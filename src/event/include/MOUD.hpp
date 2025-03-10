////////////////////////////////////////////////////////////////////////////////
// File: MOUD.hpp                                                             //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-04-10                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_MOUD_HPP_
#define EVENT_MOUD_HPP_

#include "Event.hpp"

namespace event {
/// @brief Subclass of Event used to process medication for opioid use
/// disorder
class MOUD : public Event {
private:
    class MOUDIMPL;
    std::unique_ptr<MOUDIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    MOUD(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~MOUD();

    // Copy Operations
    MOUD(MOUD const &) = delete;
    MOUD &operator=(MOUD const &) = delete;
    MOUD(MOUD &&) noexcept;
    MOUD &operator=(MOUD &&) noexcept;
};
} // namespace event
#endif // EVENT_MOUD_HPP_

////////////////////////////////////////////////////////////////////////////////
// File: FibrosisProgression.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_FIBROSISPROGRESSION_HPP_
#define EVENT_FIBROSISPROGRESSION_HPP_

#include "Event.hpp"
#include <map>
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

/// @brief Subclass of Event used to Progress HCV
class FibrosisProgression : public Event {
private:
    class FibrosisProgressionIMPL;
    std::unique_ptr<FibrosisProgressionIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    FibrosisProgression(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~FibrosisProgression();

    // Copy Operations
    FibrosisProgression(FibrosisProgression const &) = delete;
    FibrosisProgression &operator=(FibrosisProgression const &) = delete;
    FibrosisProgression(FibrosisProgression &&) noexcept;
    FibrosisProgression &operator=(FibrosisProgression &&) noexcept;
};
} // namespace event
#endif

////////////////////////////////////////////////////////////////////////////////
// File: Infections.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_INFECTIONS_HPP_
#define EVENT_INFECTIONS_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Spread Infections
class Infections : public Event {
private:
    class InfectionsIMPL;
    std::unique_ptr<InfectionsIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Infections(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~Infections();

    // Copy Operations
    Infections(Infections const &) = delete;
    Infections &operator=(Infections const &) = delete;
    Infections(Infections &&) noexcept;
    Infections &operator=(Infections &&) noexcept;
};
} // namespace event

#endif

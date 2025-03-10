////////////////////////////////////////////////////////////////////////////////
// File: Death.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_DEATH_HPP_
#define EVENT_DEATH_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

/// @brief Subclass of Event used to End the Death Process of Individuals
class Death : public Event {
private:
    class DeathIMPL;
    std::unique_ptr<DeathIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Death(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~Death();

    // Copy Operations
    Death(Death const &) = delete;
    Death &operator=(Death const &) = delete;
    Death(Death &&) noexcept;
    Death &operator=(Death &&) noexcept;
};
} // namespace event

#endif

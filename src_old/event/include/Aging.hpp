////////////////////////////////////////////////////////////////////////////////
// File: Aging.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-31                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_AGING_HPP_
#define EVENT_AGING_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Increase the Age of a Person
class Aging : public Event {
private:
    class AgingIMPL;
    std::unique_ptr<AgingIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Aging(datamanagement::ModelData &model_data);
    ~Aging();

    // Copy Operations
    Aging(Aging const &) = delete;
    Aging &operator=(Aging const &) = delete;
    Aging(Aging &&) noexcept;
    Aging &operator=(Aging &&) noexcept;
};
} // namespace event
#endif

////////////////////////////////////////////////////////////////////////////////
// File: Clearance.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-09-13                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_CLEARANCE_HPP_
#define EVENT_CLEARANCE_HPP_
#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Clear HCV Infections
class Clearance : public Event {
private:
    class ClearanceIMPL;
    std::unique_ptr<ClearanceIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Clearance(datamanagement::ModelData &model_data);
    ~Clearance();

    // Copy Operations
    Clearance(Clearance const &) = delete;
    Clearance &operator=(Clearance const &) = delete;
    Clearance(Clearance &&) noexcept;
    Clearance &operator=(Clearance &&) noexcept;
};
} // namespace event
#endif // EVENT_CLEARANCE_HPP_

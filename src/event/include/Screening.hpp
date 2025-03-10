////////////////////////////////////////////////////////////////////////////////
// File: Screening.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Screen People for Diseases
class Screening : public Event {
private:
    class ScreeningIMPL;
    std::unique_ptr<ScreeningIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Screening(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~Screening();

    // Copy Operations
    Screening(Screening const &) = delete;
    Screening &operator=(Screening const &) = delete;
    Screening(Screening &&) noexcept;
    Screening &operator=(Screening &&) noexcept;
};
} // namespace event

#endif

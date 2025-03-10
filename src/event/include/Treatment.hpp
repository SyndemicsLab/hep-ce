////////////////////////////////////////////////////////////////////////////////
// File: Treatment.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Provide Treatment to People
class Treatment : public Event {
private:
    class TreatmentIMPL;
    std::unique_ptr<TreatmentIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Treatment(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~Treatment();

    // Copy Operations
    Treatment(Treatment const &) = delete;
    Treatment &operator=(Treatment const &) = delete;
    Treatment(Treatment &&) noexcept;
    Treatment &operator=(Treatment &&) noexcept;
};
} // namespace event
#endif

////////////////////////////////////////////////////////////////////////////////
// File: VoluntaryRelinking.hpp                                               //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_VOLUNTARYRELINK_HPP_
#define EVENT_VOLUNTARYRELINK_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

/// @brief Subclass of Event used to Create Voluntary Relinkage to Treatment
/// for People
class VoluntaryRelinking : public Event {
private:
    class VoluntaryRelinkingIMPL;
    std::unique_ptr<VoluntaryRelinkingIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    VoluntaryRelinking(datamanagement::ModelData &model_data);
    ~VoluntaryRelinking();

    // Copy Operations
    VoluntaryRelinking(VoluntaryRelinking const &) = delete;
    VoluntaryRelinking &operator=(VoluntaryRelinking const &) = delete;
    VoluntaryRelinking(VoluntaryRelinking &&) noexcept;
    VoluntaryRelinking &operator=(VoluntaryRelinking &&) noexcept;
};

} // namespace event

#endif

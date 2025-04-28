////////////////////////////////////////////////////////////////////////////////
// File: HIVScreening.hpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-06                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HIVSCREENING_HPP_
#define EVENT_HIVSCREENING_HPP_

#include "Event.hpp"

namespace event {
class HIVScreening : public Event {
private:
    class HIVScreeningIMPL;
    std::unique_ptr<HIVScreeningIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    /// @param dm DataManager object containing data for the event
    /// @param decider Pseudorandom number generator for decision-making
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HIVScreening(datamanagement::ModelData &model_data);
    ~HIVScreening();

    // Copy Operations
    HIVScreening(HIVScreening const &) = delete;
    HIVScreening &operator=(HIVScreening const &) = delete;
    HIVScreening(HIVScreening &&) noexcept;
    HIVScreening &operator=(HIVScreening &&) noexcept;
};
} // namespace event

#endif

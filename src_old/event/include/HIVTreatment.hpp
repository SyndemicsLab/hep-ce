////////////////////////////////////////////////////////////////////////////////
// File: HIVTreatment.hpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-17                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HIVTREATMENT_HPP_
#define EVENT_HIVTREATMENT_HPP_

#include "Event.hpp"

namespace event {
class HIVTreatmentIMPL;

class HIVTreatment : public Event {
private:
    std::unique_ptr<HIVTreatmentIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    /// @param dm DataManager object containing data for the event
    /// @param decider Pseudorandom number generator for decision-making
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HIVTreatment(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~HIVTreatment();

    // Copy Operations
    HIVTreatment(HIVTreatment const &) = delete;
    HIVTreatment &operator=(HIVTreatment const &) = delete;
    HIVTreatment(HIVTreatment &&) noexcept;
    HIVTreatment &operator=(HIVTreatment &&) noexcept;
};
} // namespace event
#endif

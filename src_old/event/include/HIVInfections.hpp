////////////////////////////////////////////////////////////////////////////////
// File: HIVInfections.hpp                                                    //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-02-28                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HIVINFECTIONS_HPP_
#define EVENT_HIVINFECTIONS_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Spread Infections
class HIVInfections : public Event {
private:
    class HIVInfectionsIMPL;
    std::unique_ptr<HIVInfectionsIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HIVInfections(datamanagement::ModelData &model_data);
    ~HIVInfections();

    // Copy Operations
    HIVInfections(HIVInfections const &) = delete;
    HIVInfections &operator=(HIVInfections const &) = delete;
    HIVInfections(HIVInfections &&) noexcept;
    HIVInfections &operator=(HIVInfections &&) noexcept;
};
} // namespace event

#endif

////////////////////////////////////////////////////////////////////////////////
// File: HCVInfections.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HCVINFECTIONS_HPP_
#define EVENT_HCVINFECTIONS_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Spread Infections
class HCVInfections : public Event {
private:
    class HCVInfectionsIMPL;
    std::unique_ptr<HCVInfectionsIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HCVInfections(datamanagement::ModelData &model_data);
    ~HCVInfections();

    // Copy Operations
    HCVInfections(HCVInfections const &) = delete;
    HCVInfections &operator=(HCVInfections const &) = delete;
    HCVInfections(HCVInfections &&) noexcept;
    HCVInfections &operator=(HCVInfections &&) noexcept;
};
} // namespace event

#endif

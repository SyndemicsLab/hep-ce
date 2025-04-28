////////////////////////////////////////////////////////////////////////////////
// File: HCVTreatment.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HCVTREATMENT_HPP_
#define EVENT_HCVTREATMENT_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
class HCVTreatmentIMPL;

/// @brief Subclass of Event used to Provide Treatment to People
class HCVTreatment : public Event {
private:
    std::unique_ptr<HCVTreatmentIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HCVTreatment(datamanagement::ModelData &model_data);
    ~HCVTreatment();

    // Copy Operations
    HCVTreatment(HCVTreatment const &) = delete;
    HCVTreatment &operator=(HCVTreatment const &) = delete;
    HCVTreatment(HCVTreatment &&) noexcept;
    HCVTreatment &operator=(HCVTreatment &&) noexcept;
};
} // namespace event
#endif

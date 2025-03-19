////////////////////////////////////////////////////////////////////////////////
// File: HCVScreening.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-19                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_HCVSCREENING_HPP_
#define EVENT_HCVSCREENING_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Screen People for Diseases
class HCVScreening : public Event {
private:
    class HCVScreeningIMPL;
    std::unique_ptr<HCVScreeningIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    HCVScreening(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~HCVScreening();

    // Copy Operations
    HCVScreening(HCVScreening const &) = delete;
    HCVScreening &operator=(HCVScreening const &) = delete;
    HCVScreening(HCVScreening &&) noexcept;
    HCVScreening &operator=(HCVScreening &&) noexcept;
};
} // namespace event

#endif

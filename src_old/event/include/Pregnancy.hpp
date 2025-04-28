////////////////////////////////////////////////////////////////////////////////
// File: Pregnancy.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-06-13                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_PREGNANCY_HPP_
#define EVENT_PREGNANCY_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
/// @brief Subclass of Event used to Assess Pregnancy
class Pregnancy : public Event {
private:
    class PregnancyIMPL;
    std::unique_ptr<PregnancyIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Pregnancy(datamanagement::ModelData &model_data);
    ~Pregnancy();

    // Copy Operations
    Pregnancy(Pregnancy const &) = delete;
    Pregnancy &operator=(Pregnancy const &) = delete;
    Pregnancy(Pregnancy &&) noexcept;
    Pregnancy &operator=(Pregnancy &&) noexcept;
};
} // namespace event
#endif // EVENT_PREGNANCY_HPP_

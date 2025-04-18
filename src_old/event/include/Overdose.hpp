////////////////////////////////////////////////////////////////////////////////
// File: Overdose.hpp                                                         //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-10-19                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_OVERDOSE_HPP_
#define EVENT_OVERDOSE_HPP_

#include "Event.hpp"

namespace event {
/// @brief Subclass of Event used to simulate overdoses among active drug
/// users.
class Overdose : public Event {
private:
    class OverdoseIMPL;
    std::unique_ptr<OverdoseIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Overdose(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~Overdose();

    // Copy Operations
    Overdose(Overdose const &) = delete;
    Overdose &operator=(Overdose const &) = delete;
    Overdose(Overdose &&) noexcept;
    Overdose &operator=(Overdose &&) noexcept;
};
} // namespace event
#endif // EVENT_OVERDOSE_HPP_

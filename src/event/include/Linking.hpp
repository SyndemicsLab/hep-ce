////////////////////////////////////////////////////////////////////////////////
// File: Linking.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_LINKING_HPP_
#define EVENT_LINKING_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

/// @brief Subclass of Event used to Link People to Treatment
class Linking : public Event {
private:
    class LinkingIMPL;
    std::unique_ptr<LinkingIMPL> impl;

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    Linking(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~Linking();

    // Copy Operations
    Linking(Linking const &) = delete;
    Linking &operator=(Linking const &) = delete;
    Linking(Linking &&) noexcept;
    Linking &operator=(Linking &&) noexcept;
};
} // namespace event
#endif

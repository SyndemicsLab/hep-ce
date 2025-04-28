////////////////////////////////////////////////////////////////////////////////
// File: Event.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-02                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include "ConfigUtils.hpp"
#include <memory>
#include <random>
#include <vector>

// Forward Defining Person to use in Execute
namespace person {
class PersonBase;
}

// Forward Defining DeciderBase from stats project
namespace stats {
class DeciderBase;
}

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
class Event {
protected:
    virtual void DoEvent(std::shared_ptr<person::PersonBase> person,
                         datamanagement::ModelData &model_data,
                         std::shared_ptr<stats::DeciderBase> decider) = 0;

public:
    Event();
    ~Event();

    // Copy Operations
    Event(Event const &) = delete;
    Event &operator=(Event const &) = delete;
    Event(Event &&) noexcept;
    Event &operator=(Event &&) noexcept;

    int Execute(std::shared_ptr<person::PersonBase> person,
                datamanagement::ModelData &model_data,
                std::shared_ptr<stats::DeciderBase> decider);
    static double DiscountEventCost(double cost, double discount_rate,
                                    int timestep, bool annual = false);
};
} // namespace event
#endif

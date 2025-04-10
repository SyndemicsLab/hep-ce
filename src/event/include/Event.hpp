////////////////////////////////////////////////////////////////////////////////
// File: Event.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-02                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-26                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include <DataManagement/DataManagerBase.hpp>
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
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
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
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                std::shared_ptr<stats::DeciderBase> decider);
    static double DiscountEventCost(double cost, double discount_rate,
                                    int timestep, bool annual = false);

    static bool
    GetBoolFromConfig(std::string config_key,
                      std::shared_ptr<datamanagement::DataManagerBase> dm);

    static int
    GetIntFromConfig(std::string config_key,
                     std::shared_ptr<datamanagement::DataManagerBase> dm);

    static double
    GetDoubleFromConfig(std::string config_key,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        bool positive = true);
    static std::string
    GetStringFromConfig(std::string config_key,
                        std::shared_ptr<datamanagement::DataManagerBase> dm);
};
} // namespace event
#endif

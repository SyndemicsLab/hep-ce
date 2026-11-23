////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SIMULATION_HPP_
#define HEPCE_MODEL_SIMULATION_HPP_

#include <memory>
#include <string>
#include <vector>

#include <hepce/data/inputs.hpp>

#include <hepce/event/event.hpp>

namespace hepce {
namespace model {
/// Base interface for running a HEP-CE simulation.
class Hepce {
public:
    /// Destroys the simulation.
    virtual ~Hepce() = default;

    Hepce(const Hepce &) = delete;
    Hepce &operator=(const Hepce &) = delete;
    /// Creates an independent copy of the simulation.
    virtual std::unique_ptr<Hepce> clone() const = 0;

    /// Creates a simulation configured from the supplied inputs.
    /// @param inputs Simulation configuration and database inputs.
    /// @param log_name Logger name used by the simulation.
    /// @return A new simulation instance.
    static std::unique_ptr<Hepce> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    /// Runs all configured events for the supplied population.
    /// @param people Population to simulate.
    /// @param discrete_events Events applied during each timestep.
    virtual void Run(const model::People &people,
                     const event::EventList &discrete_events) = 0;
    /// Creates the configured event list.
    virtual event::EventList CreateEvents() const = 0;
    /// Creates the initial population.
    virtual model::People CreatePopulation() const = 0;

    /// Returns the configured simulation duration.
    virtual int GetDuration() const = 0;
    /// Returns the configured random seed.
    virtual int GetSeed() const = 0;

protected:
    Hepce() = default;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SIMULATION_HPP_

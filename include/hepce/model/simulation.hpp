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
class Hepce {
public:
    virtual ~Hepce() = default;

    Hepce(const Hepce &) = delete;
    Hepce &operator=(const Hepce &) = delete;
    virtual std::unique_ptr<Hepce> clone() const = 0;

    static std::unique_ptr<Hepce> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    virtual void Run(const model::People &people,
                     const event::EventList &discrete_events) = 0;
    virtual event::EventList CreateEvents() const = 0;
    virtual model::People CreatePopulation() const = 0;

    virtual int GetDuration() const = 0;
    virtual int GetSeed() const = 0;

protected:
    Hepce() = default;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SIMULATION_HPP_

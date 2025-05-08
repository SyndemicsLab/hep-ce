////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SIMULATION_HPP_
#define HEPCE_MODEL_SIMULATION_HPP_

#include <string>
#include <vector>

#include <hepce/event/event.hpp>
#include <hepce/model/person.hpp>

namespace hepce {
namespace model {
class Hepce {
public:
    virtual ~Hepce() = default;
    virtual void
    Run(std::vector<std::unique_ptr<model::Person>> &people,
        const std::vector<std::unique_ptr<event::Event>> &discrete_events) = 0;
    virtual std::vector<std::unique_ptr<event::Event>>
    CreateEvents(datamanagement::ModelData &model_data) const = 0;
    virtual std::vector<std::unique_ptr<model::Person>>
    CreatePopulation(datamanagement::ModelData &model_data) const = 0;
    virtual int LoadICValues(model::Person &person,
                             const std::vector<std::string> &icValues) = 0;

    virtual int GetDuration() const = 0;
    virtual int GetSeed() const = 0;

    static std::unique_ptr<Hepce>
    Create(datamanagement::ModelData &model_data,
           const std::string &log_name = "console");
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SIMULATION_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: simulation_internals.hpp                                             //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SIMULATIONINTERNALS_HPP_
#define HEPCE_MODEL_SIMULATIONINTERNALS_HPP_

#include <hepce/model/simulation.hpp>

#include <string>
#include <vector>

namespace hepce {
namespace model {
class HepceImpl : public virtual Hepce {
public:
    HepceImpl(const std::string &log_name);
    ~HepceImpl() = default;
    void Run(std::vector<model::Person> &people,
             const std::vector<std::unique_ptr<event::Event>> &discrete_events,
             const int duration, const int seed = 1234) override;
    std::vector<std::unique_ptr<event::Event>>
    CreateEvents(datamanagement::ModelData &model_data) const override;
    int
    CreatePersonFromTable(model::Person &person,
                          datamanagement::ModelData &model_data) const override;
    int LoadICValues(model::Person &person,
                     const std::vector<std::string> &icValues) override;

private:
    const std::string _log_name;

    const std::string GetLogName() const { return _log_name; }

    std::unique_ptr<event::Event>
    CreateEvent(std::string event_name,
                datamanagement::ModelData &model_data) const;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SIMULATIONINTERNALS_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: simulation_internals.hpp                                             //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SIMULATIONINTERNALS_HPP_
#define HEPCE_MODEL_SIMULATIONINTERNALS_HPP_

#include <hepce/model/simulation.hpp>
namespace hepce {
namespace model {
class HepceImpl : public virtual Hepce {
public:
    HepceImpl(const std::string &log_name);
    ~HepceImpl() = default;
    void Run(std::vector<model::Person> &people,
             const std::vector<event::Event> &discrete_events,
             const int duration) override;
    int CreatePersonFromTable(model::Person &person,
                              datamanagement::ModelData &model_data) override;
    int LoadICValues(model::Person &person,
                     const std::vector<std::string> &icValues) override;

private:
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SIMULATIONINTERNALS_HPP_
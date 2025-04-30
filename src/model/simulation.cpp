////////////////////////////////////////////////////////////////////////////////
// File: simulation.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-22                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/simulation.hpp>

#include "internals/simulation_internals.hpp"

namespace hepce {
namespace model {

//Factory
std::unique_ptr<Hepce> Hepce::Create(const std::string &log_name = "console") {
    return std::make_unique<HepceImpl>(log_name);
}

// Constructor
HepceImpl::HepceImpl(const std::string &log_name) {}

void HepceImpl::Run(std::vector<model::Person> &people,
                    const std::vector<event::Event> &discrete_events,
                    const int duration) {}
int HepceImpl::CreatePersonFromTable(model::Person &person,
                                     datamanagement::ModelData &model_data) {
    return 0;
}
int HepceImpl::LoadICValues(model::Person &person,
                            const std::vector<std::string> &icValues) {
    return 0;
}
} // namespace model
} // namespace hepce
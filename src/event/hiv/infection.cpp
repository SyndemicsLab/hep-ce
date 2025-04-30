////////////////////////////////////////////////////////////////////////////////
// File: infection.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/hiv/infection.hpp>

// Local Includes
#include "internals/infection_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
// Factory
std::unique_ptr<hepce::event::Event>
Infection::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<InfectionImpl>(model_data, log_name);
}

// Constructor
InfectionImpl::InfectionImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : EventBase(model_data, log_name) {}

// Execute
int InfectionImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // If already infected, exit immediately
    if (person.GetHIVDetails().hiv != data::HIV::kNone) {
        return;
    }

    // Get the probability of infection
    std::vector<double> prob = GetInfectionProbability(person);
    // Decide whether person is infected; if value == 0, infect
    if (sampler.GetDecision(prob) == 0) {
        person.InfectHIV();
    }
}
} // namespace hiv
} // namespace event
} // namespace hepce
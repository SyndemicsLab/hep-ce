////////////////////////////////////////////////////////////////////////////////
// File: infection.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/hiv/infection.hpp>

#include "hiv/internals/infection_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
InfectionImpl::InfectionImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name) {}

int InfectionImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // If already infected, exit immediately
    if (person.GetHIV() != data::HIV::NONE) {
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
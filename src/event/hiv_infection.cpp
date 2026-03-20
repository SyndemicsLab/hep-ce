////////////////////////////////////////////////////////////////////////////////
// File: hiv_infection.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Local Includes
#include "internals/hiv_infection_internals.hpp"

namespace hepce {
namespace event {
// Factory
std::unique_ptr<Event> HIVInfection::Create(const data::Inputs &inputs,
                                            const std::string &log_name) {
    return std::make_unique<HIVInfection>(inputs, log_name);
}

// Execute
void HIVInfection::Execute(model::Person &person,
                           const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
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
} // namespace event
} // namespace hepce
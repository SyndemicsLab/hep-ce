////////////////////////////////////////////////////////////////////////////////
// File: clearance.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hcv/clearance.hpp>

#include "internals/clearance_internals.hpp"
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>

namespace hepce {
namespace event {
namespace hcv {

// Factory
std::unique_ptr<hepce::event::Event>
Clearance::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<ClearanceImpl>(model_data, log_name);
}

// Constructor
ClearanceImpl::ClearanceImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : EventBase(model_data, log_name) {
    _probability =
        utils::GetDoubleFromConfig("infection.clearance_prob", model_data);

    if (_probability == 0) {
        _probability = utils::RateToProbability(0.25) / 6.0;
    }
}

// Execute
int ClearanceImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // if person isn't infected or is chronic, nothing to do
    // Also skip if person is already on treatment since we want this to
    // count as SVR
    if (person.GetHCVDetails().hcv != data::HCV::kAcute &&
        !person.GetScreeningDetails(data::InfectionType::kHcv).identified) {
        return 0;
    }
    if (sampler.GetDecision({_probability, 1 - _probability}) == 0) {
        person.ClearHCV(true);
    }
    return 0;
}
} // namespace hcv
} // namespace event
} // namespace hepce
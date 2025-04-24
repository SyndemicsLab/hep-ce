////////////////////////////////////////////////////////////////////////////////
// File: clearance.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hcv/clearance.hpp>

#include "internals/clearance_internals.hpp"
#include <hepce/utils/formatting.hpp>

namespace hepce {
namespace event {
namespace hcv {
ClearanceImpl::ClearanceImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name = "console") {
    std::string data;
    dm->GetFromConfig("infection.clearance_prob", data);
    _probability = (data.empty()) ? utils::RateToProbability(0.25) / 6.0
                                  : utils::SToDPositive(data);
}

int ClearanceImpl::Execute(model::Person &person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           model::Sampler &sampler) {
    // if person isn't infected or is chronic, nothing to do
    // Also skip if person is already on treatment since we want this to
    // count as SVR
    if (person.GetHCV() != data::HCV::ACUTE &&
        !person.HasInitiatedTreatment()) {
        return;
    }
    if (sampler.GetDecision({_probability, 1 - _probability}) == 0) {
        person.ClearHCV(true);
    }
}
} // namespace hcv
} // namespace event
} // namespace hepce
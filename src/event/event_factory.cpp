////////////////////////////////////////////////////////////////////////////////
// File: event_factory.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2026-03-19                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <hepce/data/inputs.hpp>

#include "internals/all_events.hpp"

namespace hepce {
namespace event {
std::unique_ptr<Event> EventFactory::CreateEvent(const std::string &name,
                                                 const data::Inputs &inputs,
                                                 const std::string &log_name) {
    if (name == "Aging") {
        return Aging::Create(inputs, log_name);
    }
    if (name == "BehaviorChanges") {
        return BehaviorChanges::Create(inputs, log_name);
    }
    if (name == "Clearance") {
        return HCVClearance::Create(inputs, log_name);
    }
    if (name == "Death") {
        return Death::Create(inputs, log_name);
    }
    if (name == "FibrosisProgression") {
        return Progression::Create(inputs, log_name);
    }
    if (name == "FibrosisStaging") {
        return Staging::Create(inputs, log_name);
    }
    if (name == "HCVInfection") {
        return HCVInfection::Create(inputs, log_name);
    }
    if (name == "HCVLinking") {
        return HCVLinking::Create(inputs, log_name);
    }
    if (name == "HCVScreening") {
        return HCVScreening::Create(inputs, log_name);
    }
    if (name == "HCVTreatment") {
        return HCVTreatment::Create(inputs, log_name);
    }
    if (name == "HIVInfection") {
        return HIVInfection::Create(inputs, log_name);
    }
    if (name == "HIVLinking") {
        return HIVLinking::Create(inputs, log_name);
    }
    if (name == "HIVScreening") {
        return HIVScreening::Create(inputs, log_name);
    }
    if (name == "Overdose") {
        return Overdose::Create(inputs, log_name);
    }
    if (name == "VoluntaryRelinking") {
        return VoluntaryRelink::Create(inputs, log_name);
    }
    if (name == "Pregnancy") {
        return Pregnancy::Create(inputs, log_name);
    }
    if (name == "MOUD") {
        return Moud::Create(inputs, log_name);
    }
    return nullptr;
}
} // namespace event
} // namespace hepce
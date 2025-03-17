////////////////////////////////////////////////////////////////////////////////
// File: EventFactory.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "EventFactory.hpp"
#include "AllEvents.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>

namespace event {
std::shared_ptr<Event>
EventFactory::create(std::string eventName,
                     std::shared_ptr<datamanagement::DataManagerBase> dm) {
    if (eventName == "Aging") {
        return makeEvent<Aging>(dm);
    }
    if (eventName == "BehaviorChanges") {
        return makeEvent<BehaviorChanges>(dm);
    }
    if (eventName == "Clearance") {
        return makeEvent<Clearance>(dm);
    }
    if (eventName == "Death") {
        return makeEvent<Death>(dm);
    }
    if (eventName == "FibrosisProgression") {
        return makeEvent<FibrosisProgression>(dm);
    }
    if (eventName == "FibrosisStaging") {
        return makeEvent<FibrosisStaging>(dm);
    }
    if (eventName == "HIVInfections") {
        return makeEvent<HIVInfections>(dm);
    }
    if (eventName == "HIVScreening") {
        return makeEvent<HIVScreening>(dm);
    }
    if (eventName == "Infections") {
        return makeEvent<Infections>(dm);
    }
    if (eventName == "Linking") {
        return makeEvent<Linking>(dm);
    }
    if (eventName == "Overdose") {
        return makeEvent<Overdose>(dm);
    }
    if (eventName == "Screening") {
        return makeEvent<Screening>(dm);
    }
    if (eventName == "Treatment") {
        return makeEvent<Treatment>(dm);
    }
    if (eventName == "VoluntaryRelinking") {
        return makeEvent<VoluntaryRelinking>(dm);
    }
    if (eventName == "Pregnancy") {
        return makeEvent<Pregnancy>(dm);
    }
    if (eventName == "MOUD") {
        return makeEvent<MOUD>(dm);
    }
    spdlog::get("main")->error("Unknown Event! Exiting...");
    exit(-1);
}
} // namespace event

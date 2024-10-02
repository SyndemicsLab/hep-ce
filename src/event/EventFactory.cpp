#include "EventFactory.hpp"
#include "AllEvents.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>

namespace event {
    std::shared_ptr<Event> EventFactory::create(std::string eventName) {
        if (eventName == "Aging") {
            return makeEvent<Aging>();
        }
        if (eventName == "BehaviorChanges") {
            return makeEvent<BehaviorChanges>();
        }
        if (eventName == "Clearance") {
            return makeEvent<Clearance>();
        }
        if (eventName == "Death") {
            return makeEvent<Death>();
        }
        if (eventName == "FibrosisProgression") {
            return makeEvent<FibrosisProgression>();
        }
        if (eventName == "FibrosisStaging") {
            return makeEvent<FibrosisStaging>();
        }
        if (eventName == "Infections") {
            return makeEvent<Infections>();
        }
        if (eventName == "Linking") {
            return makeEvent<Linking>();
        }
        if (eventName == "Overdose") {
            return makeEvent<Overdose>();
        }
        if (eventName == "Screening") {
            return makeEvent<Screening>();
        }
        if (eventName == "Treatment") {
            return makeEvent<Treatment>();
        }
        if (eventName == "VoluntaryRelinking") {
            return makeEvent<VoluntaryRelinking>();
        }
        if (eventName == "Pregnancy") {
            return makeEvent<Pregnancy>();
        }
        if (eventName == "MOUD") {
            return makeEvent<MOUD>();
        }
        spdlog::get("main")->error("Unknown Event! Exiting...");
        exit(-1);
    }
} // namespace event
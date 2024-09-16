#include "EventFactory.hpp"
#include "AllEvents.hpp"
#include "DataManager.hpp"
#include "spdlog/spdlog.h"

namespace event {
    std::shared_ptr<Event>
    EventFactory::create(std::shared_ptr<stats::Decider> decider,
                         std::shared_ptr<datamanagement::DataManager> dm,
                         std::string const eventName) {
        if (eventName == "Aging") {
            return makeEvent<Aging>(decider, dm, "Aging");
        }
        if (eventName == "BehaviorChanges") {
            return makeEvent<BehaviorChanges>(decider, dm, "BehaviorChanges");
        }
        if (eventName == "Clearance") {
            return makeEvent<Clearance>(decider, dm, "Clearance");
        }
        if (eventName == "Death") {
            return makeEvent<Death>(decider, dm, "Death");
        }
        if (eventName == "FibrosisProgression") {
            return makeEvent<FibrosisProgression>(decider, dm,
                                                  "FibrosisProgression");
        }
        if (eventName == "FibrosisStaging") {
            return makeEvent<FibrosisStaging>(decider, dm, "FibrosisStaging");
        }
        if (eventName == "Infections") {
            return makeEvent<Infections>(decider, dm, "Infections");
        }
        if (eventName == "Linking") {
            return makeEvent<Linking>(decider, dm, "Linking");
        }
        if (eventName == "Overdose") {
            return makeEvent<Overdose>(decider, dm, "Overdose");
        }
        if (eventName == "Screening") {
            return makeEvent<Screening>(decider, dm, "Screening");
        }
        if (eventName == "Treatment") {
            return makeEvent<Treatment>(decider, dm, "Treatment");
        }
        if (eventName == "VoluntaryRelinking") {
            return makeEvent<VoluntaryRelinking>(decider, dm,
                                                 "VoluntaryRelinking");
        }
        spdlog::get("main")->error("Unknown Event! Exiting...");
        exit(-1);
    }
} // namespace event
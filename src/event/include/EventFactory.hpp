#ifndef EVENTFACTORY_HPP_
#define EVENTFACTORY_HPP_

#include <memory>
#include <string>

#include "AllEvents.hpp"

namespace Event {

    using sharedEvent = std::shared_ptr<Event>;

    /// @brief
    /// @tparam T
    /// @param generator
    /// @param table
    /// @return
    template <typename T>
    sharedEvent makeEvent(Data::IDataTablePtr table, Data::Config &config,
                          std::shared_ptr<spdlog::logger> logger,
                          std::mt19937_64 &generator,
                          std::string name = "ProbEvent") {
        return std::make_shared<T>(generator, table, config, logger, name);
    }

    /// @brief
    /// @tparam T
    /// @param generator
    /// @param table
    /// @return
    template <typename T>
    sharedEvent makeEvent(Data::IDataTablePtr table, Data::Config &config,
                          std::shared_ptr<spdlog::logger> logger,
                          std::string name = "Event") {
        return std::make_shared<T>(table, config, logger, name);
    }

    class EventFactory {
    public:
        sharedEvent create(std::string const eventName,
                           Data::IDataTablePtr table, Data::Config &config,
                           std::shared_ptr<spdlog::logger> logger,
                           std::mt19937_64 &generator) {
            if (eventName == "Aging") {
                return makeEvent<Aging>(table, config, logger,
                                        std::string("Aging"));
            }
            if (eventName == "BehaviorChanges") {
                return makeEvent<BehaviorChanges>(table, config, logger,
                                                  generator, "BehaviorChanges");
            }
            if (eventName == "Clearance") {
                return makeEvent<Clearance>(table, config, logger, generator,
                                            "Clearance");
            }
            if (eventName == "Death") {
                return makeEvent<Death>(table, config, logger, generator,
                                        "Death");
            }
            if (eventName == "FibrosisProgression") {
                return makeEvent<FibrosisProgression>(
                    table, config, logger, generator, "FibrosisProgression");
            }
            if (eventName == "FibrosisStaging") {
                return makeEvent<FibrosisStaging>(table, config, logger,
                                                  generator, "FibrosisStaging");
            }
            if (eventName == "Infections") {
                return makeEvent<Infections>(table, config, logger, generator,
                                             "Infections");
            }
            if (eventName == "Linking") {
                return makeEvent<Linking>(table, config, logger, generator,
                                          "Linking");
            }
            if (eventName == "Overdose") {
                return makeEvent<Overdose>(table, config, logger, generator,
                                           "Overdose");
            }
            if (eventName == "Screening") {
                return makeEvent<Screening>(table, config, logger, generator,
                                            "Screening");
            }
            if (eventName == "Treatment") {
                return makeEvent<Treatment>(table, config, logger, generator,
                                            "Treatment");
            }
            if (eventName == "VoluntaryRelinking") {
                return makeEvent<VoluntaryRelinking>(
                    table, config, logger, generator, "VoluntaryRelinking");
            }
            return nullptr;
        }
    };
} // namespace Event

#endif

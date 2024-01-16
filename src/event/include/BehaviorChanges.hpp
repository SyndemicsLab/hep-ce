//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the BehaviorChanges Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_BEHAVIORCHANGES_HPP_
#define EVENT_BEHAVIORCHANGES_HPP_

#include "Event.hpp"

namespace Event {
    /// @brief Subclass of Event used to process Behavior Changes
    class BehaviorChanges : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;

        /// @brief Retrieve Transition Rates for Behavior Changes for the
        /// individual Person from the SQL Table
        /// @param person Person to retrieve transition rates for
        /// @return Vector of Transition Rates for each Behavior State
        std::vector<double>
        getTransitions(std::shared_ptr<Person::Person> person);

    public:
        BehaviorChanges(std::mt19937_64 &generator, Data::IDataTablePtr table,
                        Data::Configuration &config,
                        std::shared_ptr<spdlog::logger> logger =
                            std::make_shared<spdlog::logger>("default"),
                        std::string name = std::string("ProbEvent"))
            : ProbEvent(generator, table, config, logger, name) {}
        virtual ~BehaviorChanges() = default;
    };
} // namespace Event
#endif // EVENT_BEHAVIORCHANGES_HPP_

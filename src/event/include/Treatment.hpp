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
/// This file contains the declaration of the Treatment Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {

    struct Component {
        std::string name = "";
        double cost = 0.0;
    };

    struct Regimen {
        std::vector<Component> components = {};
        int duration = 0;
    };

    struct Course {
        std::vector<Regimen> regimens = {};
        int duration = 0;
        double withdrawalProbability = 0.0;
        double svrProbability = 0.0;
    };

    /// @brief Subclass of Event used to Provide Treatment to People
    class Treatment : public Event {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;
        bool isEligible(std::shared_ptr<Person::Person> const person) const;
        bool isEligibleFibrosisStage(Person::LiverState liverState) const;
        Course
        getTreatmentCourse(std::shared_ptr<Person::Person> const person) const;

        std::vector<Person::LiverState> eligibleLiverStates = {
            Person::LiverState::NONE};
        int eligibleTimeSinceLinked = -1;
        int eligibleTimeBehaviorChange = -1;

    public:
        Treatment(){};
        virtual ~Treatment() = default;
    };

} // namespace Event

#endif
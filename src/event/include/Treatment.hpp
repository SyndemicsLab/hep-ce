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
    template <typename T>
    std::vector<T> &operator+=(std::vector<T> &vector1,
                               const std::vector<T> &vector2) {
        vector1.insert(vector1.end(), vector2.begin(), vector2.end());
        return vector1;
    }

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
    class Treatment : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;
        bool isEligible(std::shared_ptr<Person::Person> const person) const;
        bool isEligibleFibrosisStage(Person::LiverState liverState) const;
        Course
        getTreatmentCourse(std::shared_ptr<Person::Person> const person) const;
        void populateCourses();

        std::vector<Person::LiverState> eligibleLiverStates = {
            Person::LiverState::NONE};
        int eligibleTimeSinceLinked = -1;
        int eligibleTimeBehaviorChange = -1;
        /// @brief Locate the most specific definition of a parameter in the
        /// treatment config hierarchy.
        /// @param configSections
        /// @param parameter
        /// @return
        double locateInput(std::vector<std::string> &configSections,
                           const std::string &parameter);

        std::vector<Course> courses;

    public:
        Treatment(std::mt19937_64 &generator, Data::IDataTablePtr table,
                  Data::Configuration &config,
                  std::shared_ptr<spdlog::logger> logger =
                      std::make_shared<spdlog::logger>("default"));

        std::vector<Course> getCourses() { return courses; }
        virtual ~Treatment() = default;
    };

} // namespace Event

#endif

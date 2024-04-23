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
    static std::vector<std::string>
    setupTreatmentSections(std::vector<std::string> vector1,
                           const std::vector<std::string> &vector2) {
        vector1.insert(vector1.end(), vector2.begin(), vector2.end());
        return vector1;
    }

    struct Regimen {
        int duration = 0;
        double cost = 0.0;
        double utility = 0.0;
        double withdrawalProbability = 0.0;
        double svrProbability = 0.0;
        double toxicityProbability = 0.0;
        double toxicityCost = 0.0;
        double toxicityUtility = 0.0;
    };

    struct Course {
        std::vector<Regimen> regimens = {};
        double initiationProbability = 0.0;
    };

    /// @brief Subclass of Event used to Provide Treatment to People
    class Treatment : public ProbEvent {
    private:
        std::vector<Course> courses;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;
        bool isEligible(std::shared_ptr<Person::Person> const person) const;
        bool isEligibleFibrosisStage(Person::FibrosisState fibrosisState) const;
        Course
        getTreatmentCourse(const std::shared_ptr<Person::Person> person) const;

        /// @brief Populate this Treatment event's treatment courses based on
        /// values in the config tree.
        void populateCourses();

        /// @brief
        /// @param course
        /// @param duration
        /// @return
        std::pair<Regimen, int> getCurrentRegimen(const Course &course,
                                                  int duration);

        std::vector<Person::FibrosisState> eligibleFibrosisStates = {
            Person::FibrosisState::NONE};
        int eligibleTimeSinceLinked = -1;
        int eligibleTimeBehaviorChange = -1;
        /// @brief Locate the most specific definition of a parameter in the
        /// treatment config hierarchy.
        /// @param configSections vector of strings representing config sections
        /// in reverse order of specificity
        /// @param parameter the config parameter name to find among sections
        /// @return The most-specifically-defined value of \code{parameter}
        double locateInput(std::vector<std::string> &configSections,
                           const std::string &parameter);

        void addTreatmentCost(std::shared_ptr<Person::Person> person,
                              double cost);

    public:
        Treatment(std::mt19937_64 &generator, Data::IDataTablePtr table,
                  Data::Config &config,
                  std::shared_ptr<spdlog::logger> logger =
                      std::make_shared<spdlog::logger>("default"),
                  std::string name = std::string("Treatment"));

        /// @brief Getter for treatment courses
        /// @return vector of treatment Course objects
        std::vector<Course> getCourses() { return this->courses; }

        virtual ~Treatment() = default;
    };
} // namespace Event
#endif

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
/// This file contains the declaration of the Screening Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {

    /// @brief Subclass of Event used to Screen People for Diseases
    class Screening : public ProbEvent {
    private:
        std::vector<double> backgroundProbability;
        std::vector<double> interventionProbability;
        std::vector<double> acceptTestProbability;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;

        /// @brief The Background Screening Event Undertaken on a Person
        /// @param person The Person undergoing a background Screening
        void backgroundScreen(std::shared_ptr<Person::Person> person);

        /// @brief The Intervention Screening Event Undertaken on a Person
        /// @param person The Person undergoing an Intervention Screening
        void interventionScreen(std::shared_ptr<Person::Person> person);

        /// @brief The antibody test to determine if the person is positive for
        /// HCV
        /// @param person The Person undergoing an antibody test
        /// @return Boolean True for a Positive Result, False for a Negative
        /// Result
        bool antibodyTest(std::shared_ptr<Person::Person> person,
                          std::string configKey);

        /// @brief The RNA test to determine if the person is positive for HCV
        /// @param person The Person undergoing an RNA test
        /// @return Boolean True for a Positive Result, False for a Negative
        /// Result
        bool rnaTest(std::shared_ptr<Person::Person> person,
                     std::string configKey);

        std::vector<double> getBackgroundScreeningProbability(
            std::shared_ptr<Person::Person> person);

        std::vector<double> getInterventionScreeningProbability(
            std::shared_ptr<Person::Person> person);

    public:
        using ProbEvent::ProbEvent;
        virtual ~Screening() = default;
    };

} // namespace Event

#endif

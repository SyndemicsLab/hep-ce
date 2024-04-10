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
#include <iostream>

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {
    /// @brief The type of screening that a person is currently undergoing
    enum class ScreeningType {
        // background antibody screening test
        BACKGROUND_AB = 0,
        // background RNA screening test
        BACKGROUND_RNA = 1,
        // intervention antibody screening test
        INTERVENTION_AB = 2,
        // intervention RNA screening test
        INTERVENTION_RNA = 3
    };

    /// @brief The type/cadence of intervention screening used
    enum class InterventionType {
        // no intervention screening
        NULLSCREEN = 0,
        // intervention screen once, during the first time step
        ONETIME = 1,
        // intervention screen periodically
        PERIODIC = 2
    };

    static std::unordered_map<std::string, InterventionType> interventionMap = {
        {"null", InterventionType::NULLSCREEN},
        {"one-time", InterventionType::ONETIME},
        {"periodic", InterventionType::PERIODIC}};

    /// @brief Subclass of Event used to Screen People for Diseases
    class Screening : public ProbEvent {
    private:
        std::vector<double> backgroundProbability;
        std::vector<double> interventionProbability;
        std::vector<double> acceptTestProbability;
        InterventionType interventionType;
        int interventionPeriod;

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

        void interventionDecision(std::shared_ptr<Person::Person> person);

        /// @brief Insert cost for screening of type \code{type}
        /// @param person The person who is accruing cost
        /// @param type The screening type, used to discern the cost to add
        void insertScreeningCost(std::shared_ptr<Person::Person> person,
                                 ScreeningType type);

    public:
        Screening(std::mt19937_64 &generator, Data::IDataTablePtr table,
                  Data::Configuration &config,
                  std::shared_ptr<spdlog::logger> logger =
                      std::make_shared<spdlog::logger>("default"),
                  std::string name = std::string("Screening"))
            : ProbEvent(generator, table, config, logger, name) {
            this->costCategory = Cost::CostCategory::SCREENING;
            this->interventionType = interventionMap[config.get<std::string>(
                "screening.intervention_type")];
            if (this->interventionType == InterventionType::PERIODIC) {
                this->interventionPeriod = config.get<int>("screening.period");
            }
        }
        virtual ~Screening() = default;
    };
} // namespace Event

#endif

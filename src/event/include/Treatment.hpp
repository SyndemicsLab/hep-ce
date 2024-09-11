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
namespace event {
    static std::vector<std::string>
    setupTreatmentSections(std::vector<std::string> vector1,
                           const std::vector<std::string> &vector2) {
        vector1.insert(vector1.end(), vector2.begin(), vector2.end());
        return vector1;
    }

    /// @brief Subclass of Event used to Provide Treatment to People
    class Treatment : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;
        bool isEligible(std::shared_ptr<person::Person> const person) const;
        bool isEligibleFibrosisStage(person::FibrosisState fibrosisState) const;

        std::vector<person::FibrosisState> eligibleFibrosisStates = {
            person::FibrosisState::NONE};
        int eligibleTimeSinceLinked = -1;
        int eligibleTimeBehaviorChange = -1;

        /// @brief Add the cost associated with a month of treatment
        /// @param Person the person who accrues the cost
        /// @param cost the cost associated with Person's treatment
        void addTreatmentCostAndUtility(person::PersonBase &person, double cost,
                                        double util);

        /// @brief If Person is exposed to loss to follow-up, checks if they
        /// unlink from care
        /// @param Person the Person who may unlink due to loss to follow-up
        bool isLostToFollowUp(person::PersonBase &person);

        bool initiatesTreatment(person::PersonBase &person);

        bool doesWithdraw(person::PersonBase &person,
                          Data::IDataTablePtr course);

        bool experiencedToxicity(person::PersonBase &person,
                                 Data::IDataTablePtr course);

        void chargeCostOfVisit(person::PersonBase &person);

        void chargeCostOfCourse(person::PersonBase &person,
                                Data::IDataTablePtr course);

        void quitEngagement(person::PersonBase &person);

    public:
        Treatment(std::mt19937_64 &generator, Data::IDataTablePtr table,
                  Data::Config &config,
                  std::shared_ptr<spdlog::logger> logger =
                      std::make_shared<spdlog::logger>("default"),
                  std::string name = std::string("Treatment"));

        virtual ~Treatment() = default;
    };
} // namespace event
#endif

//===-- Person.hpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the HEP-CE, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Wednesday, August 2nd 2023, 9:14:16 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef PERSON_PERSON_HPP_
#define PERSON_PERSON_HPP_

#include "Containers.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement.hpp>
#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

/// @brief Namespace containing all code pertaining to an individual Person
namespace Person {
    /// @brief A running count of the number of people in the simulation
    extern int count;

    /// @brief Class describing a Person
    class Person {
    private:
        // set person ID to the current number of total people in the runtime
        int id = count;
        Sex sex = Sex::MALE;
        bool isAlive = true;
        IdentificationStatus idStatus;
        InfectionStatus infectionStatus;
        BehaviorDetails behaviorDetails;
        LinkageDetails linkStatus;
        bool overdose = false;
        MOUDDetails moudDetails;
        PregnancyDetails pregnancyDetails;
        StagingDetails stagingDetails;
        ScreeningDetails screeningDetails;
        TreatmentDetails treatmentDetails;
        Utility utility;

    public:
        /// @brief Person age in months
        int age = 0;

        static std::map<std::string, HEPCState> hepcStateMap;
        static std::map<std::string, BehaviorClassification>
            behaviorClassificationMap;
        static std::map<std::string, LinkageType> linkageTypeMap;
        static std::map<std::string, LinkageState> linkageStateMap;

        static std::map<std::string, LiverState> liverStateMap;
        static std::map<std::string, MeasuredLiverState> measuredLiverStateMap;
        static std::map<std::string, MOUD> moudMap;
        static std::map<std::string, Sex> sexMap;

        static std::map<std::string, PregnancyState> pregnancyStateMap;

        static std::map<HEPCState, std::string> hepcStateEnumToStringMap;
        static std::map<BehaviorClassification, std::string>
            behaviorClassificationEnumToStringMap;
        static std::map<LinkageType, std::string> linkageTypeEnumToStringMap;
        static std::map<LinkageState, std::string> linkageStateEnumToStringMap;

        static std::map<LiverState, std::string> liverStateEnumToStringMap;
        static std::map<MeasuredLiverState, std::string>
            measuredLiverStateEnumToStringMap;
        static std::map<MOUD, std::string> moudEnumToStringMap;
        static std::map<Sex, std::string> sexEnumToStringMap;

        static std::map<PregnancyState, std::string>
            pregnancyStateEnumToStringMap;

        /// @brief Default constructor for Person
        Person() { count++; }

        Person(Data::IDataTablePtr dataTableRow, int simCycle);

        /// @brief Default destructor for Person
        virtual ~Person() { count--; }

        /// @brief End a Person's life and set final age
        void die();

        /// @brief increase a person's age
        void grow();

        /// @brief Infect the person
        /// @param timestep Current simulation timestep
        void infect(int tstep);

        /// @brief Clear of HCV
        /// @param timestep Current simulation timestep
        void clearHCV(int tstep);

        /// @brief Update the Liver State
        /// @param ls Current Liver State
        /// @param timestep Current simulation timestep
        void updateLiver(const LiverState &ls, int tstep);

        /// @brief Update Opioid Use Behavior Classification
        /// @param bc The intended resultant BehaviorClassification
        /// @param timestep Current simulation timestep
        void updateBehavior(const BehaviorClassification &bc, int tstep);

        /// @brief Diagnose somebody's fibrosis
        /// @return Fibrosis state that is diagnosed
        LiverState diagnoseLiver(int tstep);

        /// @brief Dignose somebody with HEPC
        /// @return HEPC state that was diagnosed
        HEPCState diagnoseHEPC(int tstep);

        /// @brief Mark somebody as having been screened this timestep
        void markScreened() { this->screeningDetails.timeOfLastScreening = 0; }

        /// @brief Set the frequency in which to screen this person
        /// @param screeningFrequency Frequency in which to screen this person
        void setScreeningFrequency(int screeningFrequency) {
            this->screeningDetails.screeningFrequency = screeningFrequency;
        }

        /// @brief Add intervention screening to this person
        void addInterventionScreening() {
            this->screeningDetails.interventionScreening = true;
        }

        /// @brief Set the Seropositivity value
        /// @param seropositivity Seropositivity status to set
        void setSeropositivity(bool seropositivity) {
            this->infectionStatus.seropositivity = seropositivity;
        }

        /// @brief Reset a Person's Link State to Unlinked
        /// @param timestep Timestep during which the Person is Unlinked
        void unlink(const int tstep) {
            if (this->linkStatus.linkState == LinkageState::LINKED) {
                this->linkStatus.linkState = LinkageState::UNLINKED;
                this->linkStatus.timeOfLinkChange = tstep;
            }
        }

        /// @brief Reset a Person's Link State to Linked
        /// @param timestep Timestep during which the Person is Linked
        /// @param linkType The linkage type the Person recieves
        void link(int tstep, LinkageType linkType) {
            this->linkStatus.linkState = LinkageState::LINKED;
            this->linkStatus.timeOfLinkChange = tstep;
            this->linkStatus.linkType = linkType;
        }

        /// @brief Mark a Person as Identified as Infected
        /// @param timestep Timestep during which Identification Occurs
        void identifyAsInfected(int tstep) {
            this->idStatus.identifiedAsPositiveInfection = true;
            this->idStatus.timeIdentified = tstep;
        }

        /// @brief Getter for the Time Since the Last Screening
        /// @return The Time Since the Last Screening
        int getTimeOfLastScreening() const {
            return this->screeningDetails.timeOfLastScreening;
        }

        /// @brief Getter for the Screening Frequency
        /// @return The Screening Frequency
        int getScreeningFrequency() const {
            return this->screeningDetails.screeningFrequency;
        }

        /// @brief Getter for the if the Person was Screened via Interventions
        /// @return Intervention Screening Status
        bool isInterventionScreened() const {
            return this->screeningDetails.interventionScreening;
        }

        /// @brief Flips the person's overdose state
        void toggleOverdose() { this->overdose = !this->overdose; }

        /// @brief Getter for the person's overdose state
        /// @return Boolean representing overdose or not
        bool getOverdose() { return this->overdose; }

        /// @brief Getter for the Liver State
        /// @return The Current Liver State
        LiverState getLiverState() const {
            return this->infectionStatus.liverState;
        }

        /// @brief Getter for the HCV State
        /// @return HCV State
        HEPCState getHEPCState() const {
            return this->infectionStatus.hepcState;
        }

        /// @brief Getter for Alive Status
        /// @return Alive Status
        bool getIsAlive() const { return this->isAlive; }

        /// @brief Getter for Behavior Classification
        /// @return Behavior Classification
        BehaviorClassification getBehaviorClassification() const {
            return this->behaviorDetails.behaviorClassification;
        }

        /// @brief Getter for time since active drug use
        /// @return Time since the person left an active drug use state
        int getTimeBehaviorChange() {
            return this->behaviorDetails.timeLastActive;
        }

        /// @brief Getter for timestep in which HCV last changed
        /// @return Time Since HCV Change
        int getTimeHEPCStateChanged() const {
            return this->infectionStatus.timeHEPCStateChanged;
        }

        /// @brief Getter for Time since Liver State Change
        /// @return Time Since Liver State Change
        int getTimeLiverStateChanged() const {
            return this->infectionStatus.timeLiverStateChanged;
        }

        /// @brief Getter for Seropositivity
        /// @return Seropositivity
        bool getSeropositivity() const {
            return this->infectionStatus.seropositivity;
        }

        /// @brief Getter for Identification Status
        /// @return Boolean Describing Indentified as Positive Status
        bool isIdentifiedAsInfected() const {
            return this->idStatus.identifiedAsPositiveInfection;
        }

        int getTimeIdentified() const { return this->idStatus.timeIdentified; }

        /// @brief Getter for Link State
        /// @return Link State
        LinkageState getLinkState() const { return this->linkStatus.linkState; }

        /// @brief Getter for Timestep in which linkage changed
        /// @return Time Link Change
        int getTimeOfLinkChange() const {
            return this->linkStatus.timeOfLinkChange;
        }

        /// @brief Getter for Linkage Type
        /// @return Linkage Type
        LinkageType getLinkageType() const { return this->linkStatus.linkType; }

        /// @brief Get the person's numeric ID
        /// @return Person's numeric ID
        int getID() const { return this->id; }

        /// @brief
        /// @return
        bool hadIncompleteTreatment() const {
            return this->treatmentDetails.incompleteTreatment;
        }

        /// @brief
        /// @return
        bool hasInitiatedTreatment() const {
            return this->treatmentDetails.initiatedTreatment;
        }

        /// @brief
        /// @return
        int getTimeOfTreatmentInitiation() const {
            return this->treatmentDetails.timeOfTreatmentInitiation;
        }

        /// @brief
        /// @param incompleteTreatment
        void setIncompleteTreatment(bool incompleteTreatment) {
            this->treatmentDetails.incompleteTreatment = incompleteTreatment;
        }

        /// @brief
        /// @param incompleteTreatment
        void setInitiatedTreatment(bool initiatedTreatment) {
            this->treatmentDetails.initiatedTreatment = initiatedTreatment;
        }

        /// @brief
        /// @param tstep
        void setTimeOfTreatmentInitiation(int tstep) {
            this->treatmentDetails.timeOfTreatmentInitiation = tstep;
        }

        /// @brief Getter for pregnancy status
        /// @return Pregnancy State
        PregnancyState getPregnancyState() {
            return this->pregnancyDetails.pregnancyState;
        }

        /// @brief Getter for timestep in which last pregnancy change happened
        /// @return Time pregnancy state last changed
        int getTimeOfPregnancyChange() {
            return this->pregnancyDetails.timeOfPregnancyChange;
        }

        /// @brief Getter for number of infants
        /// @return Number of infants born to this person
        int getInfantCount() { return this->pregnancyDetails.infantCount; }

        /// @brief Getter for number of miscarriages
        /// @return Number of miscarriages this person has experienced
        int getMiscarriageCount() {
            return this->pregnancyDetails.miscarriageCount;
        }

        /// @brief Set Person's measured liver state
        /// @param state
        void setMeasuredLiverState(MeasuredLiverState state) {
            this->stagingDetails.measuredLiverState = state;
        }

        /// @brief Getter for measured liver state
        /// @return Measured Liver State
        MeasuredLiverState getMeasuredLiverState() {
            return this->stagingDetails.measuredLiverState;
        }

        /// @brief Getter for timestep in which the last liver staging test
        /// happened
        /// @return Timestep of person's last liver staging
        int getTimeOfLastStaging() const {
            return this->stagingDetails.timeOfLastStaging;
        }

        /// @brief Getter for MOUD State
        /// @return MOUD State
        MOUD getMoudState() const { return this->moudDetails.moudState; }

        /// @brief Getter for timestep in which MOUD was started
        /// @return Time spent on MOUD
        int getTimeStartedMoud() const {
            return this->moudDetails.timeStartedMoud;
        }

        /// @brief Getter for the person's sex
        /// @return Person's sex
        Sex getSex() const { return this->sex; }

        /// @brief Set a value for a person's utility
        /// @param category The category of the utility to be updated
        /// @param value The value of the utility to be updated, bounded by 0, 1
        void setUtility(UtilityCategory category, double value);

        /// @brief Getter for the person's stratified utilities
        /// @return Person's stratified utilities
        Utility getUtility() const { return this->utility; }

        /// @brief Getter for the person's minimal and multiplicative utilities
        /// @return Minimal utility and multiplicative utility
        std::pair<double, double> getUtilities() const;
    };
} // namespace Person
#endif

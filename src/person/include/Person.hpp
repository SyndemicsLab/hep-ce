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
#include "Cost.hpp"
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
        HCCStatus hccStatus;
        Utility utility;
        std::pair<double, double> totalUtilities = {0, 0};
        Cost::CostTracker costs;
        bool boomerClassification = false;

    public:
        /// @brief Person age in months
        int age = 0;

        static std::map<std::string, HEPCState> hepcStateMap;
        static std::map<std::string, BehaviorClassification>
            behaviorClassificationMap;
        static std::map<std::string, LinkageType> linkageTypeMap;
        static std::map<std::string, LinkageState> linkageStateMap;

        static std::map<std::string, FibrosisState> fibrosisStateMap;
        static std::map<std::string, MeasuredFibrosisState>
            measuredFibrosisStateMap;
        static std::map<std::string, HCCState> hccStateMap;
        static std::map<std::string, MOUD> moudMap;
        static std::map<std::string, Sex> sexMap;

        static std::map<std::string, PregnancyState> pregnancyStateMap;

        static std::map<HEPCState, std::string> hepcStateEnumToStringMap;
        static std::map<BehaviorClassification, std::string>
            behaviorClassificationEnumToStringMap;
        static std::map<LinkageType, std::string> linkageTypeEnumToStringMap;
        static std::map<LinkageState, std::string> linkageStateEnumToStringMap;

        static std::map<FibrosisState, std::string>
            fibrosisStateEnumToStringMap;
        static std::map<MeasuredFibrosisState, std::string>
            measuredFibrosisStateEnumToStringMap;
        static std::map<MOUD, std::string> moudEnumToStringMap;
        static std::map<Sex, std::string> sexEnumToStringMap;

        static std::map<PregnancyState, std::string>
            pregnancyStateEnumToStringMap;

        /// @brief Default constructor for Person
        Person() { count++; }

        /// @brief Constructor for creating a person with pre-defined
        /// characteristics
        /// @param dataTableRow A single row from a DataTable object
        Person(Data::IDataTablePtr dataTableRow);

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
        void updateFibrosis(const FibrosisState &ls, int tstep);

        /// @brief Update Opioid Use Behavior Classification
        /// @param bc The intended resultant BehaviorClassification
        /// @param timestep Current simulation timestep
        void updateBehavior(const BehaviorClassification &bc, int tstep);

        /// @brief Diagnose somebody's fibrosis
        /// @return Fibrosis state that is diagnosed
        FibrosisState diagnoseFibrosis(int tstep);

        /// @brief Dignose somebody with HEPC
        /// @return HEPC state that was diagnosed
        HEPCState diagnoseHEPC(int tstep);

        /// @brief Getter for the number of HCV infections experienced by Person
        /// @return Number of HCV infections experienced by Person
        int getNumInfections() { return this->infectionStatus.numInfections; }

        /// @brief Add an acute clearance to the running count
        void addClearance() { this->infectionStatus.numClearances++; };

        /// @brief Get the running total of clearances for Person
        int getClearances() { return this->infectionStatus.numClearances; };

        /// @brief Mark somebody as having been screened this timestep
        void markScreened() { this->screeningDetails.timeOfLastScreening = 0; }

        /// @brief
        void addAbScreen() { this->screeningDetails.abCount++; }

        /// @brief
        void addRnaScreen() { this->screeningDetails.rnaCount++; }

        /// @brief
        int getAbCount() { return this->screeningDetails.rnaCount; }

        /// @brief
        int getRnaCount() { return this->screeningDetails.rnaCount; }

        /// @brief Getter for whether Person experienced fibtest two this cycle
        /// @return value of hadFibTestTwo
        bool hadFibTestTwo() { return this->stagingDetails.hadFibTestTwo; }

        /// @brief Set whether the person experienced fibtest two this cycle
        /// @param state New value of hadFibTestTwo
        void setHadFibTestTwo(bool state) {
            this->stagingDetails.hadFibTestTwo = state;
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
            this->linkStatus.linkCount++;
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

        /// @brief Flips the person's overdose state
        void toggleOverdose() { this->overdose = !this->overdose; }

        /// @brief Getter for the person's overdose state
        /// @return Boolean representing overdose or not
        bool getOverdose() { return this->overdose; }

        /// @brief Getter for the Fibrosis State
        /// @return The Current Fibrosis State
        FibrosisState getFibrosisState() const {
            return this->infectionStatus.fibrosisState;
        }

        /// @brief Getter for the HCV State
        /// @return HCV State
        HEPCState getHEPCState() const {
            return this->infectionStatus.hepcState;
        }

        /// @brief Set HEPC State -- used to change to chronic infection
        /// @param New HEPC State
        void setHEPCState(HEPCState hcvs) {
            this->infectionStatus.hepcState = hcvs;
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

        /// @brief Getter for Time since Fibrosis State Change
        /// @return Time Since Fibrosis State Change
        int getTimeFibrosisStateChanged() const {
            return this->infectionStatus.timeFibrosisStateChanged;
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

        /// @brief Getter for link count
        /// @return Number of times Person has linked to care
        int getLinkCount() const { return this->linkStatus.linkCount; }

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

        /// @brief Getter for whether Person has initiated treatment
        /// @return Boolean true if Person has initiated treatment, false
        /// otherwise
        bool hasInitiatedTreatment() const {
            return this->treatmentDetails.initiatedTreatment;
        }

        /// @brief Getter for the number of timesteps Person has been in
        /// treatment
        /// @return Integer number of timesteps spent in treatment
        int getTimeOfTreatmentInitiation() const {
            return this->treatmentDetails.timeOfTreatmentInitiation;
        }

        /// @brief Setter for Person's incomplete treatment state
        /// @param incompleteTreatment Boolean value for incomplete treatment
        /// state to be set
        void setIncompleteTreatment(bool incompleteTreatment) {
            this->treatmentDetails.incompleteTreatment = incompleteTreatment;
        }

        /// @brief Setter for Person's treatment initiation state
        /// @param incompleteTreatment Boolean value for initiated treatment
        /// state to be set
        void setInitiatedTreatment(bool initiatedTreatment) {
            this->treatmentDetails.initiatedTreatment = initiatedTreatment;
        }

        /// @brief Setter for treatment initiation timestep
        /// @param tstep The timestep during which treatment is initiated
        void setTimeOfTreatmentInitiation(int tstep) {
            this->treatmentDetails.timeOfTreatmentInitiation = tstep;
        }

        /// @brief Add to the tracked count of treatment initiations for Person
        void incrementTreatCount() { this->treatmentDetails.treatmentCount++; }

        /// @brief Get the tracked count of treatment initiations for Person
        int getTreatmentCount() const {
            return this->treatmentDetails.treatmentCount;
        }

        /// @brief Access whether Person can be exposed to loss to follow-up
        bool exposedToLTFU() const {
            return this->treatmentDetails.exposedToLTFU;
        }

        /// @brief Set whether Person is to be exposed to loss to follow-up
        /// @param state New value for exposedToLTFU
        void setExposedToLTFU(bool state) {
            this->treatmentDetails.exposedToLTFU = state;
        }

        /// @brief Add to end of treatment (EOT) count
        void addEOT() { this->treatmentDetails.numEOT++; }

        /// @brief Get number of treatments completed
        int getNumEOT() { return this->treatmentDetails.numEOT; }

        /// @brief Add to SVR/cure count
        void addSVR() { this->treatmentDetails.numSVR++; }

        /// @brief Get number of times Person achieved SVR
        /// @return The number of times Person reached treatment end
        int getNumSVR() { return this->treatmentDetails.numSVR; }

        /// @brief Add to adverse treatment effect coutn
        void addTox() { this->treatmentDetails.numTox++; }

        /// @brief Get number of times Person experienced adverse treatment
        /// effects
        /// @return number of adverse treatment effects experienced
        int getNumTox() { return this->treatmentDetails.numTox; }

        /// @brief Add treatment withdrawal to Person
        void addWithdrawal() { this->treatmentDetails.numWithdrawals++; }

        /// @brief Get the number of treatment withdrawals experienced by Person
        /// @return Number of treatment withdrawals
        int getWithdrawals() { return this->treatmentDetails.numWithdrawals; }

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

        /// @brief Setter for Pregnancy State
        /// @param state Person's pregnancy State
        void setPregnancyState(PregnancyState state) {
            this->pregnancyDetails.pregnancyState = state;
        }

        /// @brief Set Pregnancy State Time Change
        /// @param time Time of Pregnancy Change
        void setTimeOfPregnancyChange(int time) {
            this->pregnancyDetails.timeOfPregnancyChange = time;
        }

        /// @brief Add infants to the count
        /// @param infants Number infants to add
        void setInfantCount(int infants) {
            this->pregnancyDetails.infantCount += infants;
        }

        /// @brief Add miscarriages to the count
        /// @param miscarriages Number of miscarriages to add
        void setMiscarriageCount(int miscarriages) {
            this->pregnancyDetails.miscarriageCount += miscarriages;
        }

        /// @brief Set Person's measured fibrosis state
        /// @param state
        void setMeasuredFibrosisState(MeasuredFibrosisState state) {
            this->stagingDetails.measuredFibrosisState = state;
        }

        /// @brief Getter for measured fibrosis state
        /// @return Measured Fibrosis State
        MeasuredFibrosisState getMeasuredFibrosisState() {
            return this->stagingDetails.measuredFibrosisState;
        }

        /// @brief Setter for whether Person is genotype three
        /// @param genotype True if infection is genotype three, false
        /// otherwise
        void setGenotype(bool genotype) {
            this->infectionStatus.isGenotypeThree = genotype;
        }

        /// @brief Getter for whether Person is genotype three
        /// @return True if genotype three, false otherwise
        bool getGenotype() const {
            return this->infectionStatus.isGenotypeThree;
        }

        /// @brief Getter for timestep in which the last fibrosis
        /// staging test happened
        /// @return Timestep of person's last fibrosis staging
        int getTimeOfLastStaging() const {
            return this->stagingDetails.timeOfLastStaging;
        }

        /// @brief Getter for MOUD State
        /// @return MOUD State
        MOUD getMoudState() const { return this->moudDetails.moudState; }

        /// @brief Setter for MOUD State
        /// @param moud Person's new MOUD state
        void setMoudState(MOUD moud) { this->moudDetails.moudState = moud; }

        /// @brief Getter for timestep in which MOUD was started
        /// @return Time spent on MOUD
        int getTimeStartedMoud() const {
            return this->moudDetails.timeStartedMoud;
        }

        /// @brief Setter for timestep in which MOUD was started
        /// @param timestep Timestep during which MOUD started
        void setTimeStartedMoud(int timestep) {
            this->moudDetails.timeStartedMoud = timestep;
        }

        /// @brief Getter for the person's sex
        /// @return Person's sex
        Sex getSex() const { return this->sex; }

        /// @brief Set a value for a person's utility
        /// @param category The category of the utility to be updated
        /// @param value The value of the utility to be updated, bounded by
        /// 0, 1
        void setUtility(UtilityCategory category, double value);

        /// @brief Getter for the person's stratified utilities
        /// @return Person's stratified utilities
        Utility getUtility() const { return this->utility; }

        /// @brief Getter for the person's minimal and multiplicative
        /// utilities
        /// @return Minimal utility and multiplicative utility
        std::pair<double, double> getUtilities() const;

        /// @brief Record Person's utilities
        void measureUtilities();

        /// @brief Getter for total utilities
        std::pair<double, double> getTotalUtilities() const {
            return this->totalUtilities;
        }

        /// @brief Add a cost to the person's CostTracker object
        /// @param cost The cost to be added
        /// @param timestep The timestep during which the cost was accrued
        void addCost(Cost::Cost cost, int timestep);

        /// @brief Getter for the Person's costs
        /// @return Cost::CostTracker containing this person's costs
        Cost::CostTracker getCosts() const { return this->costs; }

        bool isBoomer() { return this->boomerClassification; }

        void setBoomerClassification(bool status) {
            this->boomerClassification = status;
        }
    };
} // namespace Person
#endif

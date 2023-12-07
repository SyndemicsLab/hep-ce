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

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utils.hpp"

/// @brief Namespace containing all code pertaining to an individual Person
namespace Person {
    /// @brief A running count of the number of people in the simulation
    extern int count;

    /// @brief HEP-C Infection States
    enum class HEPCState {
        /// No HCV infection
        NONE,
        /// New HCV infection; sub-6 months infected
        ACUTE,
        /// Long-term HCV infection
        CHRONIC
    };

    /// @brief Opioid Usage Behavior Classification
    /// @details There are five possible possible usage classifications.
    enum class BehaviorClassification {
        /// No history of opioid use
        NEVER,
        /// Former non-injection opioid use
        FORMER_NONINJECTION,
        /// Former injection opioid use
        FORMER_INJECTION,
        /// Non-injection opioid use
        NONINJECTION,
        /// Injection opioid use
        INJECTION
    };

    /// @brief Screening type that led to linkage
    enum class LinkageType {
        /// Linked through background screening
        BACKGROUND,
        /// Linked through intervention screening
        INTERVENTION
    };

    /// @brief Status of Linkage
    enum class LinkageState {
        /// Person has never been linked to care
        NEVER,
        /// Person is currently linked to care
        LINKED,
        /// Person was previously linked to care, but is not currently linked
        UNLINKED
    };

    /// @brief Classification of Liver Disease Stage
    /// @details HCV infection causes liver fibrosis and cirrhosis and increases
    /// the risk of development of hepatocellular carcinoma (HCC).
    /// These states strictly increase, with the possibility of progressing to
    /// HCC being possible at any time from stage F3 and higher.
    enum class LiverState {
        /// No adverse liver effects
        NONE,
        /// No scarring
        F0,
        /// Mild liver scarring
        F1,
        /// Scarring has occurred and extends outside the liver area
        F2,
        /// Fibrosis spreading and forming bridges with other fibrotic liver
        /// areas
        F3,
        /// Cirrhosis or advanced scarring
        F4,
        /// Symptomatic cirrhosis; overt complications
        DECOMP,
        /// Early-stage hepatocellular carcinoma
        EHCC,
        /// Late-stage hepatocellular carcinoma
        LHCC
    };

    /// @brief Clinically staged liver fibrosis stage
    enum class MeasuredLiverState {
        /// Person has never been screened before
        NONE,
        /// Person is measured to be either F0 or F1
        F01,
        /// Person is measured to be either F2 or F3
        F23,
        /// Person is measured to be F4
        F4,
        /// Person has decompensated liver
        DECOMP
    };

    /// @brief Opioid Use Disorder Treatment States (MOUDs)
    enum class MOUD {
        /// Never in MOUD
        NONE,
        /// Currently in MOUD
        CURRENT,
        /// Recently dropped out of MOUD
        POST
    };

    /// @brief Biological Sex
    enum class Sex {
        /// Assigned male at birth
        MALE,
        /// Assigned female at birth
        FEMALE
    };

    /// @brief Pregnancy Classification
    /// @details There are three possible pregnancy states.
    enum class PregnancyState {
        /// Never pregnant
        NONE,
        /// Actively pregnant
        PREGNANT,
        /// Post-pregnancy
        POSTPARTUM
    };

    /// @brief Attributes describing Identification
    struct IdentificationStatus {
        bool identifiedAsPositiveInfection = false;
        int timeIdentified = -1;
    };

    /// @brief Attributes describing an Infection
    struct InfectionStatus {
        HEPCState hepcState = HEPCState::NONE;
        LiverState liverState = LiverState::NONE;
        bool seropositivity = false;
        int timeHEPCStateChanged = 0;
        int timeLiverStateChanged = 0;
    };

    /// @brief Attributes describing drug use behavior
    struct BehaviorDetails {
        BehaviorClassification behaviorClassification =
            BehaviorClassification::NEVER;
        int timeLastActive = -1;
    };

    /// @brief Attributes describing Linkage
    struct LinkageDetails {
        LinkageState linkState = LinkageState::NEVER;
        int timeOfLinkChange = -1;
        LinkageType linkType = LinkageType::BACKGROUND;
    };

    /// @brief Attributes describing MOUD status
    struct MOUDDetails {
        MOUD moudState = MOUD::NONE;
        int timeStartedMoud = -1;
    };

    /// @brief Attributes describing pregnancy
    struct PregnancyDetails {
        PregnancyState pregnancyState = PregnancyState::NONE;
        int timeOfPregnancyChange = -1;
        int infantCount = 0;
        int miscarriageCount = 0;
    };

    /// @brief Person attributes describing clinically assessed liver stage
    struct StagingDetails {
        MeasuredLiverState measuredLiverState = MeasuredLiverState::NONE;
        int timeOfLastStaging = -1;
    };

    struct ScreeningDetails {
        // -1 if never screened, otherwise [0, currentTimestep-1)
        int timeOfLastScreening = -1;
        int screeningFrequency = -1; // -1 if screened only once and never again
        bool interventionScreening = false;
    };

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
        bool incompleteTreatment = false;
        MOUDDetails moudDetails;
        PregnancyDetails pregnancyDetails;
        StagingDetails stagingDetails;
        ScreeningDetails screeningDetails;

    public:
        /// @brief Person age in years
        double age = 0;

        static std::unordered_map<std::string, HEPCState> hepcStateMap;
        static std::unordered_map<std::string, BehaviorClassification>
            behaviorClassificationMap;
        static std::unordered_map<std::string, LinkageType> linkageTypeMap;
        static std::unordered_map<std::string, LinkageState> linkageStateMap;

        static std::unordered_map<std::string, LiverState> liverStateMap;
        static std::unordered_map<std::string, MeasuredLiverState>
            measuredLiverStateMap;
        static std::unordered_map<std::string, MOUD> moudMap;
        static std::unordered_map<std::string, Sex> sexMap;

        static std::unordered_map<std::string, PregnancyState>
            pregnancyStateMap;

        static std::unordered_map<HEPCState, std::string>
            hepcStateEnumToStringMap;
        static std::unordered_map<BehaviorClassification, std::string>
            behaviorClassificationEnumToStringMap;
        static std::unordered_map<LinkageType, std::string>
            linkageTypeEnumToStringMap;
        static std::unordered_map<LinkageState, std::string>
            linkageStateEnumToStringMap;

        static std::unordered_map<LiverState, std::string>
            liverStateEnumToStringMap;
        static std::unordered_map<MeasuredLiverState, std::string>
            measuredLiverStateEnumToStringMap;
        static std::unordered_map<MOUD, std::string> moudEnumToStringMap;
        static std::unordered_map<Sex, std::string> sexEnumToStringMap;

        static std::unordered_map<PregnancyState, std::string>
            pregnancyStateEnumToStringMap;

        /// @brief Default constructor for Person
        Person() { count++; }

        Person(std::vector<std::string> dataTableRow, int simCycle);

        /// @brief Default destructor for Person
        virtual ~Person() { count--; }

        /// @brief End a Person's life and set final age
        void die();

        /// @brief increase a person's age
        void grow();

        /// @brief Infect the person
        /// @param timestep Current simulation timestep
        void infect(int timestep);

        /// @brief Clear of HCV
        /// @param timestep Current simulation timestep
        void clearHCV(int timestep);

        /// @brief Update the Liver State
        /// @param ls Current Liver State
        /// @param timestep Current simulation timestep
        void updateLiver(const LiverState &ls, int timestep);

        /// @brief Update Opioid Use Behavior Classification
        /// @param bc The intended resultant BehaviorClassification
        /// @param timestep Current simulation timestep
        void updateBehavior(const BehaviorClassification &bc, int timestep);

        /// @brief Diagnose somebody's fibrosis
        /// @return Fibrosis state that is diagnosed
        LiverState diagnoseLiver(int timestep);

        /// @brief Dignose somebody with HEPC
        /// @return HEPC state that was diagnosed
        HEPCState diagnoseHEPC(int timestep);

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
        void unlink(const int timestep) {
            if (this->linkStatus.linkState == LinkageState::LINKED) {
                this->linkStatus.linkState = LinkageState::UNLINKED;
                this->linkStatus.timeOfLinkChange = timestep;
            }
        }

        /// @brief Reset a Person's Link State to Linked
        /// @param timestep Timestep during which the Person is Linked
        /// @param linkType The linkage type the Person recieves
        void link(int timestep, LinkageType linkType) {
            this->linkStatus.linkState = LinkageState::LINKED;
            this->linkStatus.timeOfLinkChange = timestep;
            this->linkStatus.linkType = linkType;
        }

        /// @brief Mark a Person as Identified as Infected
        /// @param timestep Timestep during which Identification Occurs
        void identifyAsInfected(int timestep) {
            this->idStatus.identifiedAsPositiveInfection = true;
            this->idStatus.timeIdentified = timestep;
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
            return this->incompleteTreatment;
        }

        /// @brief
        /// @param incompleteTreatment
        /// @return
        void setIncompleteTreatment(bool incompleteTreatment) {
            this->incompleteTreatment = incompleteTreatment;
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

        /// @brief Getter for measured liver state
        /// @return Measured Liver State
        MeasuredLiverState getMeasuredLiverState() {
            return this->stagingDetails.measuredLiverState;
        }

        /// @brief Getter for timestep in which the last liver staging test
        /// happened
        /// @return Timestep of person's last liver staging
        int getTimeOfLastStaging() {
            return this->stagingDetails.timeOfLastStaging;
        }

        /// @brief Getter for MOUD State
        /// @return MOUD State
        MOUD getMoudState() { return this->moudDetails.moudState; }

        /// @brief Getter for timestep in which MOUD was started
        /// @return Time spent on MOUD
        int getTimeStartedMoud() { return this->moudDetails.timeStartedMoud; }

        Sex getSex() { return this->sex; }
    };
} // namespace Person
#endif

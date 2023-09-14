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

/// @brief Namespace containing all code pertaining to an individual Person
namespace Person {
    /// @brief A running count of the number of people in the simulation
    extern int count;

    /// @brief Classification of Liver Disease Stage
    /// @details HCV infection causes liver fibrosis and cirrhosis and increases
    /// the risk of development of hepatocellular carcinoma (HCC).
    /// These states strictly increase, with the possibility of progressing to
    /// HCC being possible at any time from stage F3 and higher.
    enum class LiverState { NONE, F0, F1, F2, F3, F4, DECOMP, EHCC, LHCC };

    /// @brief HEP-C Infection States
    enum class HEPCState { NONE, ACUTE, CHRONIC };

    /// @brief Usage Behavior Classification
    /// @details There are five possible possible usage classifications:
    /// - No History of Opioid Use
    /// - Former Non-injection Opioid Use
    /// - Former Injection Opioid Use
    /// - Non-injection Opioid Use
    /// - Injection Opioid Use
    enum class BehaviorClassification {
        NEVER,
        FORMER_NONINJECTION,
        FORMER_INJECTION,
        NONINJECTION,
        INJECTION
    };

    /// @brief Screening type that lead to Linkage
    enum class LinkageType { BACKGROUND, INTERVENTION };

    /// @brief Status of Linkage
    enum class LinkageState { NEVER, LINKED, UNLINKED };

    /// @brief class describing a Person
    class Person {
    private:
        int id = count;

        // -1 if never screened, otherwise [0, currentTimestep-1)
        int timeSinceLastScreening = -1;
        int screeningFrequency = -1; // -1 if screened only once and never again
        bool interventionScreening = false;
        bool seropositivity = false;

        /// @brief Attributes describing Identification
        struct IdentificationStatus {
            bool identifiedAsPositiveInfection = false;
            int timeIdentified = -1;
        };
        IdentificationStatus idStatus;

        /// @brief Attributes describing an Infection
        struct InfectionStatus {
            HEPCState hepcState = HEPCState::NONE;
            LiverState liverState = LiverState::NONE;
            int timeSinceHEPCStateChange = 0;
            int timeSinceLiverStateChange = 0;
        };
        InfectionStatus infectionStatus;

        bool isAlive = true;
        BehaviorClassification behaviorClassification =
            BehaviorClassification::NEVER;

        /// @brief Attributes describing Linkage
        struct LinkageDetails {
            LinkageState linkState = LinkageState::NEVER;
            int timeLinkChange = -1;
            LinkageType linkType = LinkageType::BACKGROUND;
        };

        LinkageDetails linkStatus;

    public:
        double age = 0;

        Person() { count++; }
        virtual ~Person() { count--; }

        /// @brief End a Person's life and set final age
        void die();

        /// @brief increase a person's age
        void grow();

        /// @brief Infect the person
        void infect();

        /// @brief Clear of HCV
        void clearHCV();

        /// @brief Update the Liver State
        /// @param ls Current Liver State
        void updateLiver(const LiverState &ls);

        /// @brief Update Opioid Use Behavior Classification
        /// @param bc The intended resultant BehaviorClassification
        void updateBehavior(const BehaviorClassification &bc);

        /// @brief Diagnose somebody's fibrosis
        /// @return Fibrosis state that is diagnosed
        LiverState diagnoseLiver(int timestep);

        /// @brief Dignose somebody with HEPC
        /// @return HEPC state that was diagnosed
        HEPCState diagnoseHEPC(int timestep);

        /// @brief Mark somebody as having been screened this timestep
        void markScreened() { this->timeSinceLastScreening = 0; }

        /// @brief Set the frequency in which to screen this person
        /// @param screeningFrequency Frequency in which to screen this person
        void setScreeningFrequency(int screeningFrequency) {
            this->screeningFrequency = screeningFrequency;
        }

        /// @brief Add intervention screening to this person
        void addInterventionScreening() { this->interventionScreening = true; }

        /// @brief Set the Seropositivity value
        /// @param seropositivity Seropositivity status to set
        void setSeropositivity(bool seropositivity) {
            this->seropositivity = seropositivity;
        }

        /// @brief Reset a Person's Link State to Unlinked
        /// @param timestep Timestep during which the Person is Unlinked
        void unlink(const int timestep) {
            if (this->linkStatus.linkState == LinkageState::LINKED) {
                this->linkStatus.linkState = LinkageState::UNLINKED;
                this->linkStatus.timeLinkChange = timestep;
            }
        }

        /// @brief Reset a Person's Link State to Linked
        /// @param timestep Timestep during which the Person is Linked
        /// @param linkType The linkage type the Person recieves
        void link(int timestep, LinkageType linkType) {
            this->linkStatus.linkState = LinkageState::LINKED;
            this->linkStatus.timeLinkChange = timestep;
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
        int getTimeSinceLastScreening() { return this->timeSinceLastScreening; }

        /// @brief Getter for the Screening Frequency
        /// @return The Screening Frequency
        int getScreeningFrequency() { return this->screeningFrequency; }

        /// @brief Getter for the if the Person was Screened via Interventions
        /// @return Intervention Screening Status
        bool isInterventionScreened() { return this->interventionScreening; }

        /// @brief Getter for the Liver State
        /// @return The Current Liver State
        LiverState getLiverState() { return this->infectionStatus.liverState; }

        /// @brief Getter for the HCV State
        /// @return HCV State
        HEPCState getHEPCState() { return this->infectionStatus.hepcState; }

        /// @brief Getter for Alive Status
        /// @return Alive Status
        bool getIsAlive() { return this->isAlive; }

        /// @brief Getter for Behavior Classification
        /// @return Behavior Classification
        BehaviorClassification getBehaviorClassification() {
            return this->behaviorClassification;
        }

        /// @brief Getter for Time since HCV Change
        /// @return Time Since HCV Change
        int getTimeSinceHEPCStateChange() {
            return this->infectionStatus.timeSinceHEPCStateChange;
        }

        /// @brief Getter for Time since Liver State Change
        /// @return Time Since Liver State Change
        int getTimeSinceLiverStateChange() {
            return this->infectionStatus.timeSinceLiverStateChange;
        }

        /// @brief Getter for Seropositivity
        /// @return Seropositivity
        bool getSeropositivity() { return this->seropositivity; }

        /// @brief Getter for Identification Status
        /// @return Boolean Describing Indentified as Positive Status
        bool isIdentifiedAsInfected() {
            return this->idStatus.identifiedAsPositiveInfection;
        }

        /// @brief Getter for Link State
        /// @return Link State
        LinkageState getLinkState() { return this->linkStatus.linkState; }

        /// @brief Getter for Time Link Change
        /// @return Time Link Change
        int getTimeLinkChange() { return this->linkStatus.timeLinkChange; }

        /// @brief Getter for Linkage Type
        /// @return Linkage Type
        LinkageType getLinkageType() { return this->linkStatus.linkType; }

        /// @brief Get the person's numeric ID
        int getID() { return this->id; }
    };
} // namespace Person
#endif

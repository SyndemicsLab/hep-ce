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
        int id;

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

        struct InfectionStatus {
            HEPCState hepcState = HEPCState::NONE;
            LiverState liverState = LiverState::NONE;
            int timeSinceHEPCStateChange = 0;
            int timeSinceLiverStateChange = 0;
        };
        InfectionStatus infectionStatus;

        bool isAlive = true;
        BehaviorClassification behaviorClassification;

        /// @brief Attributes describing Linkage
        struct LinkageDetails {
            LinkageState linkState = LinkageState::NEVER;
            int timeLinkChange = -1;
            LinkageType linkType = LinkageType::BACKGROUND;
        };

        LinkageDetails linkStatus;

    public:
        double age = 0;

        Person(){};
        virtual ~Person() = default;

        /// @brief End a Person's life and set final age
        void die();

        /// @brief increase a person's age
        void grow();

        /// @brief Infect the person
        void infect();

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

        /// @brief
        /// @param timestep
        void unlink(int timestep) {
            if (this->linkStatus.linkState == LinkageState::LINKED) {
                this->linkStatus.linkState = LinkageState::UNLINKED;
                this->linkStatus.timeLinkChange = timestep;
            }
        }

        /// @brief
        /// @param timestep
        /// @param linkType
        void link(int timestep, LinkageType linkType) {
            this->linkStatus.linkState = LinkageState::LINKED;
            this->linkStatus.timeLinkChange = timestep;
            this->linkStatus.linkType = linkType;
        }

        /// @brief
        /// @param timestep
        void identifyAsInfected(int timestep) {
            this->idStatus.identifiedAsPositiveInfection = true;
            this->idStatus.timeIdentified = timestep;
        }

        /// @brief
        /// @return
        int getTimeSinceLastScreening() { return this->timeSinceLastScreening; }

        /// @brief
        /// @return
        int getScreeningFrequency() { return this->screeningFrequency; }

        /// @brief
        /// @return
        bool isInterventionScreened() { return this->interventionScreening; }

        /// @brief
        /// @return
        LiverState getLiverState() { return this->infectionStatus.liverState; }

        /// @brief
        /// @return
        HEPCState getHEPCState() { return this->infectionStatus.hepcState; }

        /// @brief
        /// @return
        bool getIsAlive() { return this->isAlive; }

        /// @brief
        /// @return
        BehaviorClassification getBehaviorClassification() {
            return this->behaviorClassification;
        }

        /// @brief
        /// @return
        int getTimeSinceHEPCStateChange() {
            return this->infectionStatus.timeSinceHEPCStateChange;
        }

        /// @brief
        /// @return
        int getTimeSinceLiverStateChange() {
            return this->infectionStatus.timeSinceLiverStateChange;
        }

        /// @brief
        /// @return
        bool getSeropositivity() { return this->seropositivity; }

        /// @brief
        /// @return
        bool isIdentifiedAsInfected() {
            return this->idStatus.identifiedAsPositiveInfection;
        }

        /// @brief
        /// @return
        LinkageState getLinkState() { return this->linkStatus.linkState; }

        /// @brief
        /// @return
        int getTimeLinkChange() { return this->linkStatus.timeLinkChange; }

        /// @brief
        /// @return
        LinkageType getLinkageType() { return this->linkStatus.linkType; }
    };
} // namespace Person
#endif

//===-- Person.hpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the HEP-CE, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
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

/// @brief Namespace containing all code pertaining to a Person
namespace Person{

/// @brief Fibrosis States
enum FibrosisState { 
    NONE,
    F0,
    F1,
    F2,
    F3,
    F4,
    DECOMP
};

/// @brief HEP-C Infection States
enum HEPCState {
    NONE,
    ACUTE,
    CHRONIC
};

/// @brief Person's Living Status
enum Alive {
    NO,
    YES
};

/// @brief Opioid Usage Status
enum BehaviorState {
    NEVER,
    CURRENT,
    FORMER
};

/// @brief Screening type that lead to Linkage
enum LinkageType {
    BACKGROUND,
    INTERVENTION
};

/// @brief Status of Linkage
enum LinkageState {
    NEVER,
    LINKED,
    UNLINKED
};

/// @brief class describing a Person
class Person {
private:
    int id;
    
    int timeSinceLastScreening = -1; // -1 if never screened, otherwise [0, currentTimestep-1)
    int screeningFrequency = -1; // -1 if screened only once and never again
    bool interventionScreening = false;
    bool seropositivity = false;

    /// @brief Attributes describing Identification
    struct IdentificationStatus {
        bool identifiedAsPositiveInfection = false;
        int timeIdentified = -1;
    };
    IdentificationStatus idStatus;

    FibrosisState fibState = FibrosisState::NONE;
    HEPCState hepceState = HEPCState::NONE;
    Alive alive = Alive::YES;
    BehaviorState behaviorState = BehaviorState::NEVER;

    /// @brief Attributes describing Linkage
    struct LinkageDetails {
        LinkageState linkState = LinkageState::NEVER;
        int timeLinkChange = -1;
        LinkageType linkType = LinkageType::BACKGROUND;

    };

    LinkageDetails linkStatus;

public:
    uint32_t age = 0;

    Person(){};
    virtual ~Person() = default;

    /// @brief End a Person's life and set final age
    void die();

    /// @brief increase a person's age
    void grow();

    /// @brief Infect the person
    void infect();

    /// @brief Update Opioid use behavior
    void updateBehavior();

    /// @brief Diagnose somebody's fibrosis
    /// @return Fibrosis state that is diagnosed
    FibrosisState diagnoseFibrosis();

    /// @brief Dignose somebody with HEPC
    /// @return HEPC state that was diagnosed
    HEPCState diagnoseHEPC();

    /// @brief Mark somebody as having been screened this timestep
    void markScreened() { this->timeSinceLastScreening = 0; }

    /// @brief Set the frequency in which to screen this person
    /// @param screeningFrequency Frequency in which to screen this person
    void setScreeningFrequency(int screeningFrequency) { this->screeningFrequency = screeningFrequency; }

    /// @brief Add intervention screening to this person
    void addInterventionScreening() {this->interventionScreening = true; }

    /// @brief Set the Seropositivity value
    /// @param seropositivity Seropositivity status to set
    void setSeropositivity(bool seropositivity) { this->seropositivity = seropositivity; }

    /// @brief 
    /// @param timestep 
    void unlink(int timestep) { 
        if(this->linkStatus.linkState == LinkageState::LINKED){ 
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
    int getScreeningFrequency() { return this-> screeningFrequency; }

    /// @brief 
    /// @return 
    bool isInterventionScreened() { return this->interventionScreening; }

    /// @brief 
    /// @return 
    FibrosisState getFibrosisState() { return this->fibState; }

    /// @brief 
    /// @return 
    HEPCState getHEPCState() { return this->hepceState; }

    /// @brief 
    /// @return 
    Alive getIsAlive() { return this->alive; }

    /// @brief 
    /// @return 
    BehaviorState getBehaviorState() { return this->behaviorState; }

    /// @brief 
    /// @return 
    bool getSeropositivity(){ return this->seropositivity; }

    /// @brief 
    /// @return 
    bool isIdentifiedAsInfected() { return this->idStatus.identifiedAsPositiveInfection; }

    /// @brief 
    /// @return 
    LinkageState getLinkState() { return this->linkStatus.linkState; }

    /// @brief 
    /// @return 
    LinkageType getLinkageType() { return this->linkStatus.linkType; }
};

}
#endif
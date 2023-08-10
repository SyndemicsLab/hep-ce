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

namespace Person{

enum FibrosisState { 
    NONE,
    F0,
    F1,
    F2,
    F3,
    F4,
    DECOMP
};

enum HEPCState {
    NONE,
    ACUTE,
    CHRONIC
};

enum Alive {
    NO,
    YES
};

enum BehaviorState {
    NEVER,
    CURRENT,
    FORMER
};

enum LinkageState {
    LINKED,
    UNLINKED
};

class Person {
private:
    int id;
    
    int timeSinceLastScreening = -1; // -1 if never screened, otherwise [0, currentTimestep-1)
    int screeningFrequency = -1; // -1 if screened only once and never again
    bool interventionScreening = false;
    bool seropositivity = false;

    FibrosisState fibState = FibrosisState::NONE;
    HEPCState hepceState = HEPCState::NONE;
    Alive alive = Alive::YES;
    BehaviorState behaviorState = BehaviorState::NEVER;
    LinkageState linkState = LinkageState::UNLINKED;

public:
    uint32_t age = 0;

    Person(){};
    virtual ~Person() = default;

    void die();
    void grow();
    void infect();
    void updateBehavior();
    FibrosisState diagnoseFibrosis();
    HEPCState diagnoseHEPC();
    void markScreened() { this->timeSinceLastScreening = 0; }
    void setScreeningFrequency(int screeningFrequency) { this->screeningFrequency = screeningFrequency; }
    void addInterventionScreening() {this->interventionScreening = true; }
    void setSeropositivity(bool seropositivity) { this->seropositivity = seropositivity; }
    void unlink() { this->linkState = LinkageState::UNLINKED; }
    void link() { this->linkState = LinkageState::LINKED; }

    int getTimeSinceLastScreening() { return this->timeSinceLastScreening; }
    int getScreeningFrequency() { return this-> screeningFrequency; }
    bool isInterventionScreened() { return this->interventionScreening; }
    FibrosisState getFibrosisState() { return this->fibState; }
    HEPCState getHEPCState() { return this->hepceState; }
    Alive getIsAlive() { return this->alive; }
    BehaviorState getBehaviorState() { return this->behaviorState; }
    bool getSeropositivity(){ return this->seropositivity; }
};

}
#endif
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

#ifndef PERSON_HPP_
#define PERSON_HPP_

#include <cstdint>
#include <string>

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

class Person {
private:
    int id;
    FibrosisState fibState = FibrosisState::NONE;
    HEPCState hepceState = HEPCState::NONE;
    Alive alive = Alive::YES;
    BehaviorState behaviorState = BehaviorState::NEVER;

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

    FibrosisState getFibrosisState();
    HEPCState getHEPCState();
    Alive getIsAlive();
    BehaviorState getBehaviorState();
};

#endif
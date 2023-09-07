//===-- PersonTest.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, August 15th 2023, 8:42:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <gtest/gtest.h>

#include "Person.hpp"

TEST(PersonCreation, DefaultConstructor) {
    Person::Person *person = new Person::Person();
    EXPECT_TRUE(person);
    delete (person);
}

TEST(PersonDeath, DieFunction) {
    Person::Person person;
    EXPECT_TRUE(person.getIsAlive());
    person.die();
    EXPECT_FALSE(person.getIsAlive());
}

TEST(PersonGrowth, GrowNormally) {
    Person::Person person;
    person.grow();
    EXPECT_EQ(person.age, 1);
}

TEST(PersonGrowth, DeathAge) {
    Person::Person person;
    person.die();
    person.grow();
    EXPECT_EQ(person.age, 0);
}

TEST(PersonInfect, InfectNormally) {
    Person::Person person;
    person.infect();
    EXPECT_EQ(person.getHEPCState(), Person::HEPCState::ACUTE);
    EXPECT_EQ(person.getTimeSinceHEPCStateChange(), 0);
    EXPECT_TRUE(person.getSeropositivity());

    EXPECT_EQ(person.getLiverState(), Person::LiverState::NONE);
    EXPECT_EQ(person.getTimeSinceLiverStateChange(), 0);
}

TEST(PersonInfect, ResistMultiHCVInfect) {
    Person::Person person;
    person.diagnoseHEPC(5);
    person.setSeropositivity(false);
    person.infect();
    EXPECT_EQ(person.getHEPCState(), Person::HEPCState::ACUTE);
    EXPECT_FALSE(person.getSeropositivity());
}

TEST(PersonInfect, ResistMultiFibrosisInfect) {
    Person::Person person;
    person.diagnoseLiver(5);
    person.infect();
    EXPECT_EQ(person.getHEPCState(), Person::HEPCState::ACUTE);
    EXPECT_EQ(person.getLiverState(), Person::LiverState::F0);
}

TEST(PersonLink, LinkNormally) {
    Person::Person person;
    int timestep = 5;
    Person::LinkageType linkType = Person::LinkageType::BACKGROUND;
    person.link(timestep, linkType);
    EXPECT_EQ(person.getLinkState(), Person::LinkageState::LINKED);
    EXPECT_EQ(person.getTimeLinkChange(), timestep);
    EXPECT_EQ(person.getLinkageType(), linkType);
}

TEST(PersonUnlink, UnlinkNormally) {
    Person::Person person;
    int linkTimestep = 5;
    person.link(linkTimestep, Person::LinkageType::BACKGROUND);
    int unlinkTimestep = 10;
    person.unlink(unlinkTimestep);
    EXPECT_EQ(person.getLinkState(), Person::LinkageState::UNLINKED);
    EXPECT_EQ(person.getTimeLinkChange(), unlinkTimestep);
}

TEST(PersonUnlink, UnableToUnlink) {
    Person::Person person;
    person.unlink(10);
    EXPECT_EQ(person.getTimeLinkChange(), -1);
}

TEST(PersonBehavior, BehaviorUpdate) {
    Person::Person person;
    // newly-generated person starts in the NEVER state
    EXPECT_EQ(person.getBehaviorClassification(),
              Person::BehaviorClassification::NEVER);
    // change to the noninjection opioid usage state
    person.updateBehavior(Person::BehaviorClassification::NONINJECTION);
    EXPECT_EQ(person.getBehaviorClassification(),
              Person::BehaviorClassification::NONINJECTION);
}

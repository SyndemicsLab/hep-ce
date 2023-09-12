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
#include <memory>
#include <vector>

#include "Person.hpp"

TEST(PersonCreation, DefaultConstructor) {
    Person::Person *person = new Person::Person();
    EXPECT_TRUE(person);
    delete (person);
}

TEST(PersonCreation, PersonCountID) {
    std::vector<std::shared_ptr<Person::Person>> population;
    // check population count
    for (int i = 0; i < 10; ++i) {
        population.push_back(std::make_shared<Person::Person>());
    }
    EXPECT_EQ(Person::count, 10);
    // check individual person IDs
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(population[i]->getID(), i);
    }
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
    EXPECT_EQ(person.age, 1.0);
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
    // change to the NONINJECTION opioid usage state
    person.updateBehavior(Person::BehaviorClassification::NONINJECTION);
    EXPECT_EQ(person.getBehaviorClassification(),
              Person::BehaviorClassification::NONINJECTION);
}

TEST(PersonBehavior, InvalidTransition) {
    Person::Person person;
    person.updateBehavior(Person::BehaviorClassification::NONINJECTION);
    // cannot transition back to NEVER
    person.updateBehavior(Person::BehaviorClassification::NEVER);
    EXPECT_EQ(person.getBehaviorClassification(),
              Person::BehaviorClassification::NONINJECTION);
}

TEST(PersonLiver, LiverUpdate) {
    Person::Person person;
    // people start in the NONE state
    EXPECT_EQ(person.getLiverState(), Person::LiverState::NONE);
    // change to a higher disease state
    person.updateLiver(Person::LiverState::F4);
    EXPECT_EQ(person.getLiverState(), Person::LiverState::F4);
}

TEST(PersonLiver, InvalidTransition) {
    Person::Person person;
    person.updateLiver(Person::LiverState::F4);
    // cannot transition to a lower disease state
    person.updateLiver(Person::LiverState::F3);
    EXPECT_EQ(person.getLiverState(), Person::LiverState::F4);
}

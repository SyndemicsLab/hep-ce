//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the Unit Tests for Person and its Subclasses.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "Person.hpp"

TEST(PersonCreation, DefaultConstructor) {
    person::Person *person = new person::Person();
    EXPECT_TRUE(person);
    delete (person);
}

TEST(PersonCreation, PersonCountID) {
    std::vector<std::shared_ptr<person::Person>> population;
    // check population count
    for (int i = 0; i < 10; ++i) {
        population.push_back(std::make_shared<person::Person>());
    }
    EXPECT_EQ(person::count, 10);
    // check individual person IDs
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(population[i]->getID(), i);
    }
}

TEST(PersonDeath, DieFunction) {
    person::Person person;
    EXPECT_TRUE(person.getIsAlive());
    person.die();
    EXPECT_FALSE(person.getIsAlive());
}

TEST(PersonGrowth, GrowNormally) {
    person::Person person;
    person.grow();
    EXPECT_EQ(person.age, 1);
}

TEST(PersonGrowth, DeathAge) {
    person::Person person;
    person.die();
    person.grow();
    EXPECT_EQ(person.age, 0);
}

TEST(PersonInfect, InfectNormally) {
    person::Person person;
    person.infect(0);
    EXPECT_EQ(person.getHCV(), person::HCV::ACUTE);
    EXPECT_EQ(person.getTimeHCVChanged(), 0);
    EXPECT_TRUE(person.getSeropositive());

    EXPECT_EQ(person.getFibrosisState(), person::FibrosisState::F0);
    EXPECT_EQ(person.getTimeFibrosisStateChanged(), 0);
}

TEST(PersonInfect, ResistMultiHCVInfect) {
    person::Person person;
    person.diagnoseHEPC(5);
    person.setSeropositive(false);
    person.infect(0);
    EXPECT_EQ(person.getHCV(), person::HCV::ACUTE);
    EXPECT_FALSE(person.getSeropositive());
}

TEST(PersonInfect, ResistMultiFibrosisInfect) {
    person::Person person;
    person.diagnoseFibrosis(5);
    person.infect(0);
    EXPECT_EQ(person.getHCV(), person::HCV::ACUTE);
    EXPECT_EQ(person.getFibrosisState(), person::FibrosisState::F0);
}

TEST(PersonLink, LinkNormally) {
    person::Person person;
    int tstep = 5;
    person::LinkageType linkType = person::LinkageType::BACKGROUND;
    person.link(tstep, linkType);
    EXPECT_EQ(person.getLinkState(), person::LinkageState::LINKED);
    EXPECT_EQ(person.getTimeOfLinkChange(), tstep);
    EXPECT_EQ(person.getLinkageType(), linkType);
}

TEST(PersonUnlink, UnlinkNormally) {
    person::Person person;
    int linkTimestep = 5;
    person.link(linkTimestep, person::LinkageType::BACKGROUND);
    int unlinkTimestep = 10;
    person.unlink(unlinkTimestep);
    EXPECT_EQ(person.getLinkState(), person::LinkageState::UNLINKED);
    EXPECT_EQ(person.getTimeOfLinkChange(), unlinkTimestep);
}

TEST(PersonUnlink, UnableToUnlink) {
    person::Person person;
    person.unlink(10);
    EXPECT_EQ(person.getTimeOfLinkChange(), 0);
}

TEST(PersonBehavior, BehaviorUpdate) {
    person::Person person;
    // newly-generated person starts in the NEVER state
    EXPECT_EQ(person.getBehavior(), person::Behavior::NEVER);
    // change to the NONINJECTION opioid usage state
    person.updateBehavior(person::Behavior::NONINJECTION, 0);
    EXPECT_EQ(person.getBehavior(), person::Behavior::NONINJECTION);
}

TEST(PersonBehavior, InvalidTransition) {
    person::Person person;
    person.updateBehavior(person::Behavior::NONINJECTION, 0);
    // cannot transition back to NEVER
    person.updateBehavior(person::Behavior::NEVER, 0);
    EXPECT_EQ(person.getBehavior(), person::Behavior::NONINJECTION);
}

TEST(PersonLiver, FibrosisUpdate) {
    person::Person person;
    // people start in the NONE state
    EXPECT_EQ(person.getFibrosisState(), person::FibrosisState::NONE);
    // change to a higher disease state
    person.updateFibrosis(person::FibrosisState::F4, 0);
    EXPECT_EQ(person.getFibrosisState(), person::FibrosisState::F4);
}

TEST(PersonLiver, InvalidTransition) {
    person::Person person;
    person.updateFibrosis(person::FibrosisState::F4, 0);
    // cannot transition to a lower disease state
    person.updateFibrosis(person::FibrosisState::F3, 0);
    EXPECT_EQ(person.getFibrosisState(), person::FibrosisState::F4);
}

TEST(PersonUtility, SetUtility) {
    person::Person person;
    person.setUtility(0.75);
    person.setUtility(0.5);
    person::UtilityTracker utilityTracker = person.getUtility();
    EXPECT_EQ(0.5, utilityTracker.minUtil);
    EXPECT_EQ(0.375, utilityTracker.multUtil);
}

TEST(PersonPrint, PrintPerson) {
    person::Person person;
    std::cout << person;
}

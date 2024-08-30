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
    person.infect(0);
    EXPECT_EQ(person.getHCV(), Person::HCV::ACUTE);
    EXPECT_EQ(person.getTimeHCVChanged(), 0);
    EXPECT_TRUE(person.getSeropositive());

    EXPECT_EQ(person.getFibrosisState(), Person::FibrosisState::F0);
    EXPECT_EQ(person.getTimeFibrosisStateChanged(), 0);
}

TEST(PersonInfect, ResistMultiHCVInfect) {
    Person::Person person;
    person.diagnoseHEPC(5);
    person.setSeropositive(false);
    person.infect(0);
    EXPECT_EQ(person.getHCV(), Person::HCV::ACUTE);
    EXPECT_FALSE(person.getSeropositive());
}

TEST(PersonInfect, ResistMultiFibrosisInfect) {
    Person::Person person;
    person.diagnoseFibrosis(5);
    person.infect(0);
    EXPECT_EQ(person.getHCV(), Person::HCV::ACUTE);
    EXPECT_EQ(person.getFibrosisState(), Person::FibrosisState::F0);
}

TEST(PersonLink, LinkNormally) {
    Person::Person person;
    int tstep = 5;
    Person::LinkageType linkType = Person::LinkageType::BACKGROUND;
    person.link(tstep, linkType);
    EXPECT_EQ(person.getLinkState(), Person::LinkageState::LINKED);
    EXPECT_EQ(person.getTimeOfLinkChange(), tstep);
    EXPECT_EQ(person.getLinkageType(), linkType);
}

TEST(PersonUnlink, UnlinkNormally) {
    Person::Person person;
    int linkTimestep = 5;
    person.link(linkTimestep, Person::LinkageType::BACKGROUND);
    int unlinkTimestep = 10;
    person.unlink(unlinkTimestep);
    EXPECT_EQ(person.getLinkState(), Person::LinkageState::UNLINKED);
    EXPECT_EQ(person.getTimeOfLinkChange(), unlinkTimestep);
}

TEST(PersonUnlink, UnableToUnlink) {
    Person::Person person;
    person.unlink(10);
    EXPECT_EQ(person.getTimeOfLinkChange(), 0);
}

TEST(PersonBehavior, BehaviorUpdate) {
    Person::Person person;
    // newly-generated person starts in the NEVER state
    EXPECT_EQ(person.getBehavior(), Person::Behavior::NEVER);
    // change to the NONINJECTION opioid usage state
    person.updateBehavior(Person::Behavior::NONINJECTION, 0);
    EXPECT_EQ(person.getBehavior(), Person::Behavior::NONINJECTION);
}

TEST(PersonBehavior, InvalidTransition) {
    Person::Person person;
    person.updateBehavior(Person::Behavior::NONINJECTION, 0);
    // cannot transition back to NEVER
    person.updateBehavior(Person::Behavior::NEVER, 0);
    EXPECT_EQ(person.getBehavior(), Person::Behavior::NONINJECTION);
}

TEST(PersonLiver, FibrosisUpdate) {
    Person::Person person;
    // people start in the NONE state
    EXPECT_EQ(person.getFibrosisState(), Person::FibrosisState::NONE);
    // change to a higher disease state
    person.updateFibrosis(Person::FibrosisState::F4, 0);
    EXPECT_EQ(person.getFibrosisState(), Person::FibrosisState::F4);
}

TEST(PersonLiver, InvalidTransition) {
    Person::Person person;
    person.updateFibrosis(Person::FibrosisState::F4, 0);
    // cannot transition to a lower disease state
    person.updateFibrosis(Person::FibrosisState::F3, 0);
    EXPECT_EQ(person.getFibrosisState(), Person::FibrosisState::F4);
}

TEST(PersonUtility, SetUtility) {
    Person::Person person;
    person.setUtility(0.75);
    person.setUtility(0.5);
    Person::UtilityTracker utilityTracker = person.getUtility();
    EXPECT_EQ(0.5, utilityTracker.minUtil);
    EXPECT_EQ(0.375, utilityTracker.multUtil);
}

TEST(PersonPrint, PrintPerson) {
    Person::Person person;
    std::cout << person;
}

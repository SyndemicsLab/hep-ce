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
#include <DataManagement/DataManager.hpp>

class PersonTest : public ::testing::Test {
protected:
    person::PersonBase testPerson;
    void SetUp() override {
        std::shared_ptr<datamanagement::DataManager> dm =
            std::make_shared<datamanagement::DataManager>();
        person::PersonBase person(4321, dm);
        testPerson = person;
    }
    void TearDown() override {}
};

// TEST_F(PersonTest, Grow) {
//     testPerson.SetAge(10);
//     testPerson.Grow();
//     int result = testPerson.GetAge();
//     EXPECT_EQ(11, result);
// }

// TEST_F(PersonTest, Die) {
//     EXPECT_TRUE(testPerson.GetIsAlive());
//     testPerson.Die();
//     EXPECT_TRUE(!testPerson.GetIsAlive());
// }

// TEST_F(PersonTest, Infect) {
//     int timesInfected = testPerson.GetTimesInfected();
//     testPerson.Infect();
//     EXPECT_EQ(timesInfected + 1, testPerson.GetTimesInfected());
// }

// TEST_F(PersonTest, ClearHCV) {
//     testPerson.SetHCV(person::HCV::ACUTE);
//     testPerson.ClearHCV();
//     EXPECT_EQ(person::HCV::NONE, testPerson.GetHCV());
// }

// TEST_F(PersonTest, UpdateFibrosis) {
//     testPerson.UpdateFibrosis(person::FibrosisState::F4);
//     EXPECT_EQ(person::FibrosisState::F4, testPerson.GetFibrosisState());
// }

// TEST_F(PersonTest, UpdateBehavior) {
//     testPerson.UpdateBehavior(person::Behavior::FORMER_INJECTION);
//     EXPECT_EQ(person::Behavior::FORMER_INJECTION, testPerson.GetBehavior());
// }

// TEST_F(PersonTest, DiagnoseFibrosis) {
//     person::FibrosisState data = person::FibrosisState::F3;
//     testPerson.DiagnoseFibrosis(data);
//     EXPECT_EQ(person::FibrosisState::F3, testPerson.GetFibrosisState());
// }

// TEST_F(PersonTest, DiagnoseHEPC) {
//     person::HCV data = person::HCV::CHRONIC;
//     testPerson.DiagnoseHEPC(data);
//     EXPECT_EQ(person::HCV::CHRONIC, testPerson.GetHCV());
// }

// TEST_F(PersonTest, AddClearance) {
//     int before = testPerson.GetClearances();
//     testPerson.AddClearance();
//     EXPECT_EQ(before + 1, testPerson.GetClearances());
// }

// TEST_F(PersonTest, MarkScreened) {
//     testPerson.MarkScreened();
//     EXPECT_EQ(0, testPerson.GetTimeSinceScreened());
// }

// TEST_F(PersonTest, AddAbScreen) {
//     int before = testPerson.GetNumABTests();
//     testPerson.AddAbScreen();
//     EXPECT_EQ(before + 1, testPerson.GetNumABTests());
// }

// TEST_F(PersonTest, AddRNAScreen) {
//     int before = testPerson.GetNumRNATests();
//     testPerson.AddRnaScreen();
//     EXPECT_EQ(before + 1, testPerson.GetNumRNATests());
// }

// TEST_F(PersonTest, LinkAndUnlink) {
//     testPerson.Link(person::LinkageType::INTERVENTION);
//     EXPECT_EQ(person::LinkageState::LINKED, testPerson.GetLinkState());
//     testPerson.Unlink();
//     EXPECT_EQ(person::LinkageState::UNLINKED, testPerson.GetLinkState());
// }

// TEST_F(PersonTest, IdentifyAsInfected) {
//     testPerson.IdentifyAsInfected();
//     EXPECT_TRUE(testPerson.IsIdentifiedAsInfected());
// }

// TEST_F(PersonTest, AddCost) {
//     cost::Cost cost = {cost::CostCategory::MISC, "testCost", 12.34};
//     testPerson.AddCost(cost);
//     cost::CostTracker tracker = testPerson.GetCosts();
//     cost::Cost result =
//     tracker.getCosts()[testPerson.GetCurrentTimestep()][0];
//     EXPECT_EQ(result.name, "testCost");
// }

// TEST_F(PersonTest, ToggleOverdose) {
//     bool before = testPerson.GetCurrentlyOverdosing();
//     testPerson.ToggleOverdose();
//     bool after = testPerson.GetCurrentlyOverdosing();
//     EXPECT_FALSE(before == after);
// }

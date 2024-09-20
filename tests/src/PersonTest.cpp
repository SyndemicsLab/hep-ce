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
#include "PersonFactory.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManager.hpp>

class PersonTest : public ::testing::Test {
private:
    void RegisterLogger() {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

protected:
    std::shared_ptr<person::PersonBase> testPerson;
    void SetUp() override {
        RegisterLogger();
        std::shared_ptr<datamanagement::DataManager> dm =
            std::make_unique<datamanagement::DataManager>();
        person::PersonFactory pfactory;
        testPerson = pfactory.create();
        testPerson->CreatePersonFromTable(4321, dm);
    }
    void TearDown() override { spdlog::drop("main"); }
};

// Functionality Tests

// Getters and Setters Tests
TEST_F(PersonTest, ID) { EXPECT_EQ(4321, testPerson->GetID()); }

TEST_F(PersonTest, CurrentTimestep) {
    EXPECT_EQ(0, testPerson->GetCurrentTimestep());
    testPerson->Grow();
    EXPECT_EQ(1, testPerson->GetCurrentTimestep());
}

TEST_F(PersonTest, Age) {
    int age = 28;
    testPerson->SetAge(age);
    EXPECT_EQ(age, testPerson->GetAge());
}

TEST_F(PersonTest, TimesInfected) {
    EXPECT_EQ(0, testPerson->GetTimesInfected());
    testPerson->Infect();
    EXPECT_EQ(1, testPerson->GetTimesInfected());
}

TEST_F(PersonTest, Clearances) {
    EXPECT_EQ(0, testPerson->GetClearances());
    testPerson->AddClearance();
    EXPECT_EQ(1, testPerson->GetClearances());
}

TEST_F(PersonTest, Withdrawals) {
    EXPECT_EQ(0, testPerson->GetWithdrawals());
    testPerson->AddWithdrawal();
    EXPECT_EQ(1, testPerson->GetWithdrawals());
}

TEST_F(PersonTest, Toxicities) {
    EXPECT_EQ(0, testPerson->GetToxicReactions());
    testPerson->AddToxicReaction();
    EXPECT_EQ(1, testPerson->GetToxicReactions());
}

TEST_F(PersonTest, CompletedTreatments) {
    EXPECT_EQ(0, testPerson->GetCompletedTreatments());
    testPerson->AddCompletedTreatment();
    EXPECT_EQ(1, testPerson->GetCompletedTreatments());
}

TEST_F(PersonTest, SVRs) {
    EXPECT_EQ(0, testPerson->GetSVRs());
    testPerson->AddSVR();
    EXPECT_EQ(1, testPerson->GetSVRs());
}

TEST_F(PersonTest, ABTests) {
    EXPECT_EQ(0, testPerson->GetNumABTests());
    testPerson->AddAbScreen();
    EXPECT_EQ(1, testPerson->GetNumABTests());
}

TEST_F(PersonTest, RNATests) {
    EXPECT_EQ(0, testPerson->GetNumRNATests());
    testPerson->AddRnaScreen();
    EXPECT_EQ(1, testPerson->GetNumRNATests());
}

TEST_F(PersonTest, TimeOfLastScreening) {
    EXPECT_EQ(-1, testPerson->GetTimeOfLastScreening());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    testPerson->MarkScreened();
    EXPECT_EQ(3, testPerson->GetTimeOfLastScreening());
}

TEST_F(PersonTest, TimeOfSinceScreened) {
    testPerson->MarkScreened();
    EXPECT_EQ(0, testPerson->GetTimeOfLastScreening());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeSinceScreened());
}

TEST_F(PersonTest, Overdose) {
    EXPECT_EQ(false, testPerson->GetCurrentlyOverdosing());
    testPerson->ToggleOverdose();
    EXPECT_EQ(true, testPerson->GetCurrentlyOverdosing());
    testPerson->ToggleOverdose();
    EXPECT_EQ(false, testPerson->GetCurrentlyOverdosing());
}

TEST_F(PersonTest, NumOverdoses) {
    EXPECT_EQ(0, testPerson->GetNumberOfOverdoses());
    testPerson->ToggleOverdose();
    EXPECT_EQ(1, testPerson->GetNumberOfOverdoses());
    testPerson->ToggleOverdose();
    EXPECT_EQ(1, testPerson->GetNumberOfOverdoses());
    testPerson->ToggleOverdose();
    EXPECT_EQ(2, testPerson->GetNumberOfOverdoses());
}

TEST_F(PersonTest, DeathReason) {
    person::DeathReason reason = person::DeathReason::LIVER;
    testPerson->SetDeathReason(reason);
    EXPECT_EQ(reason, testPerson->GetDeathReason());
}

TEST_F(PersonTest, FibrosisState) {
    person::FibrosisState state = person::FibrosisState::F3;
    testPerson->SetTrueFibrosisState(state);
    EXPECT_EQ(state, testPerson->GetFibrosisState());
}
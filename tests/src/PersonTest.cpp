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

#include "DataManagerMock.hpp"
#include "Person.hpp"
#include "PersonFactory.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using ::testing::_;
using ::testing::Return;

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
        std::shared_ptr<datamanagement::MOCKDataManager> dm =
            std::make_unique<datamanagement::MOCKDataManager>();
        EXPECT_CALL(*dm, SelectCustomCallback(_, _, _, _))
            .WillRepeatedly(Return(0));
        person::PersonFactory pfactory;
        testPerson = pfactory.create();
        testPerson->CreatePersonFromTable(4321, NULL);
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
    EXPECT_EQ(0, testPerson->GetTimesHCVInfected());
    testPerson->InfectHCV();
    EXPECT_EQ(1, testPerson->GetTimesHCVInfected());
}

TEST_F(PersonTest, Clearances) {
    EXPECT_EQ(0, testPerson->GetHCVClearances());
    testPerson->AddHCVClearance();
    EXPECT_EQ(1, testPerson->GetHCVClearances());
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
    EXPECT_EQ(0, testPerson->GetNumberOfABTests());
    testPerson->AddAbScreen();
    EXPECT_EQ(1, testPerson->GetNumberOfABTests());
}

TEST_F(PersonTest, RNATests) {
    EXPECT_EQ(0, testPerson->GetNumberOfRNATests());
    testPerson->AddRnaScreen();
    EXPECT_EQ(1, testPerson->GetNumberOfRNATests());
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
    EXPECT_EQ(3, testPerson->GetTimeSinceLastScreening());
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
    EXPECT_EQ(state, testPerson->GetTrueFibrosisState());
}

TEST_F(PersonTest, HCV) {
    person::HCV state = person::HCV::CHRONIC;
    testPerson->SetHCV(state);
    EXPECT_EQ(state, testPerson->GetHCV());
}

TEST_F(PersonTest, IsAlive) {
    EXPECT_EQ(true, testPerson->IsAlive());
    testPerson->Die();
    EXPECT_EQ(false, testPerson->IsAlive());
}

TEST_F(PersonTest, Behavior) {
    person::Behavior state = person::Behavior::INJECTION;
    testPerson->SetBehavior(state);
    EXPECT_EQ(state, testPerson->GetBehavior());
}

TEST_F(PersonTest, TimeBehaviorChange) {
    EXPECT_EQ(0, testPerson->GetTimeBehaviorChange());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(0, testPerson->GetTimeBehaviorChange());
    person::Behavior state = person::Behavior::INJECTION;
    testPerson->SetBehavior(state);
    EXPECT_EQ(3, testPerson->GetTimeBehaviorChange());
}

TEST_F(PersonTest, TimeHCVChanged) {
    EXPECT_EQ(0, testPerson->GetTimeHCVChanged());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(0, testPerson->GetTimeHCVChanged());
    person::HCV state = person::HCV::CHRONIC;
    testPerson->SetHCV(state);
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeHCVChanged());
}

TEST_F(PersonTest, TimeSinceHCVChanged) {
    EXPECT_EQ(0, testPerson->GetTimeSinceHCVChanged());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeSinceHCVChanged());
    person::HCV state = person::HCV::CHRONIC;
    testPerson->SetHCV(state);
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(2, testPerson->GetTimeSinceHCVChanged());
}

TEST_F(PersonTest, TimeFibrosisChanged) {
    EXPECT_EQ(0, testPerson->GetTimeTrueFibrosisStateChanged());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(0, testPerson->GetTimeTrueFibrosisStateChanged());
    person::FibrosisState state = person::FibrosisState::F3;
    testPerson->SetTrueFibrosisState(state);
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeTrueFibrosisStateChanged());
}

TEST_F(PersonTest, TimeSinceStaging) {
    EXPECT_EQ(0, testPerson->GetTimeSinceFibrosisStaging());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeSinceFibrosisStaging());
    person::MeasuredFibrosisState state = person::MeasuredFibrosisState::F23;
    testPerson->DiagnoseFibrosis(state);
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(2, testPerson->GetTimeSinceFibrosisStaging());
}

TEST_F(PersonTest, Seropositivity) {
    EXPECT_EQ(false, testPerson->GetSeropositivity());
    testPerson->SetSeropositivity(true);
    EXPECT_EQ(true, testPerson->GetSeropositivity());
}

TEST_F(PersonTest, TimeHCVIdentified) {
    EXPECT_EQ(0, testPerson->GetTimeHCVIdentified());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(0, testPerson->GetTimeHCVIdentified());
    testPerson->DiagnoseHCV();
    EXPECT_EQ(3, testPerson->GetTimeHCVIdentified());
}

TEST_F(PersonTest, LinkState) {
    EXPECT_EQ(person::LinkageState::NEVER, testPerson->GetLinkState());
    testPerson->Link(person::LinkageType::BACKGROUND);
    EXPECT_EQ(person::LinkageState::LINKED, testPerson->GetLinkState());
}

TEST_F(PersonTest, TimeLinkChanged) {
    EXPECT_EQ(0, testPerson->GetTimeOfLinkChange());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(0, testPerson->GetTimeOfLinkChange());
    testPerson->Link(person::LinkageType::BACKGROUND);
    EXPECT_EQ(3, testPerson->GetTimeOfLinkChange());
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeOfLinkChange());
}

TEST_F(PersonTest, TimeSinceLinkChange) {
    EXPECT_EQ(0, testPerson->GetTimeSinceLinkChange());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeSinceLinkChange());
    testPerson->Link(person::LinkageType::BACKGROUND);
    EXPECT_EQ(0, testPerson->GetTimeSinceLinkChange());
    testPerson->Grow();
    EXPECT_EQ(1, testPerson->GetTimeSinceLinkChange());
}

TEST_F(PersonTest, LinkCount) {
    EXPECT_EQ(0, testPerson->GetLinkCount());
    testPerson->Link(person::LinkageType::BACKGROUND);
    testPerson->Unlink();
    testPerson->Link(person::LinkageType::BACKGROUND);
    testPerson->Unlink();
    testPerson->Link(person::LinkageType::BACKGROUND);
    testPerson->Unlink();
    EXPECT_EQ(3, testPerson->GetLinkCount());
}

TEST_F(PersonTest, LinkType) {
    EXPECT_EQ(person::LinkageType::BACKGROUND, testPerson->GetLinkageType());
    person::LinkageType type = person::LinkageType::INTERVENTION;
    testPerson->Link(type);
    EXPECT_EQ(type, testPerson->GetLinkageType());
}

TEST_F(PersonTest, TimeOfTreatmentInitiation) {
    EXPECT_EQ(0, testPerson->GetTimeOfTreatmentInitiation());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(0, testPerson->GetTimeOfTreatmentInitiation());
    testPerson->InitiateTreatment();
    EXPECT_EQ(3, testPerson->GetTimeOfTreatmentInitiation());
}

TEST_F(PersonTest, TimeSinceTreatmentInitiation) {
    EXPECT_EQ(0, testPerson->GetTimeSinceTreatmentInitiation());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(3, testPerson->GetTimeSinceTreatmentInitiation());
    testPerson->InitiateTreatment();
    EXPECT_EQ(0, testPerson->GetTimeSinceTreatmentInitiation());
}
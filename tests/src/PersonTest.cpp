////////////////////////////////////////////////////////////////////////////////
// File: PersonTest.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "DataManagerMock.hpp"
#include "Person.hpp"
#include "PersonFactory.hpp"
#include "Utils.hpp"
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
    EXPECT_EQ(0, testPerson->GetAcuteHCVClearances());
    testPerson->AddAcuteHCVClearance();
    EXPECT_EQ(1, testPerson->GetAcuteHCVClearances());
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
    testPerson->UpdateTrueFibrosis(state);
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
    EXPECT_EQ(-1, testPerson->GetTimeBehaviorChange());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(-1, testPerson->GetTimeBehaviorChange());
    person::Behavior state = person::Behavior::INJECTION;
    testPerson->SetBehavior(state);
    EXPECT_EQ(3, testPerson->GetTimeBehaviorChange());
}

TEST_F(PersonTest, TimeHCVChanged) {
    EXPECT_EQ(-1, testPerson->GetTimeHCVChanged());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(-1, testPerson->GetTimeHCVChanged());
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
    EXPECT_EQ(-1, testPerson->GetTimeTrueFibrosisStateChanged());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(-1, testPerson->GetTimeTrueFibrosisStateChanged());
    person::FibrosisState state = person::FibrosisState::F3;
    testPerson->UpdateTrueFibrosis(state);
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
    EXPECT_EQ(-1, testPerson->GetTimeHCVIdentified());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(-1, testPerson->GetTimeHCVIdentified());
    testPerson->Diagnose();
    EXPECT_EQ(3, testPerson->GetTimeHCVIdentified());
}

TEST_F(PersonTest, LinkState) {
    EXPECT_EQ(person::LinkageState::NEVER, testPerson->GetLinkState());
    testPerson->Link(person::LinkageType::BACKGROUND);
    EXPECT_EQ(person::LinkageState::LINKED, testPerson->GetLinkState());
}

TEST_F(PersonTest, TimeLinkChanged) {
    EXPECT_EQ(-1, testPerson->GetTimeOfLinkChange());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(-1, testPerson->GetTimeOfLinkChange());
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
    EXPECT_EQ(person::LinkageType::NA, testPerson->GetLinkageType());
    person::LinkageType type = person::LinkageType::INTERVENTION;
    testPerson->Link(type);
    EXPECT_EQ(type, testPerson->GetLinkageType());
}

TEST_F(PersonTest, TimeOfTreatmentInitiation) {
    EXPECT_EQ(-1, testPerson->GetTimeOfTreatmentInitiation());
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    EXPECT_EQ(-1, testPerson->GetTimeOfTreatmentInitiation());
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

TEST_F(PersonTest, CurrentUtility) {
    // test default utility matches expectations
    std::pair<double, double> expected_utils = {1.0, 1.0};
    EXPECT_EQ(expected_utils, testPerson->GetUtility());

    // test setting utilities multiple times
    expected_utils = {0.8, 0.8};
    testPerson->SetUtility(0.8, utility::UtilityCategory::BACKGROUND);
    EXPECT_EQ(expected_utils, testPerson->GetUtility());

    expected_utils = {0.6, 0.48};
    testPerson->SetUtility(0.6, utility::UtilityCategory::TREATMENT);
    EXPECT_EQ(expected_utils, testPerson->GetUtility());

    expected_utils = {0.6, 0.6};
    testPerson->SetUtility(1.0, utility::UtilityCategory::BACKGROUND);
    EXPECT_EQ(expected_utils, testPerson->GetUtility());
}

TEST_F(PersonTest, TotalUtility) {
    // test initialized state
    // min, mult, discount_min, discount_mult
    person::LifetimeUtility expected = {0.0, 0.0, 0.0, 0.0};
    EXPECT_EQ(expected, testPerson->GetTotalUtility());

    double discount_rate = 0.025;

    // simple test of values set by AccumulateTotalUtility
    std::pair<double, double> undiscounted_utils = testPerson->GetUtility();
    double discounted_util =
        Utils::discount(1.0, discount_rate, testPerson->GetCurrentTimestep());
    std::pair<double, double> discounted_utils = {discounted_util,
                                                  discounted_util};
    expected = {1.0, 1.0, discounted_util, discounted_util};
    testPerson->AccumulateTotalUtility(undiscounted_utils, discounted_utils);
    EXPECT_EQ(expected, testPerson->GetTotalUtility());

    // slighly more complex case
    testPerson->Grow();
    discounted_util =
        Utils::discount(1.0, discount_rate, testPerson->GetCurrentTimestep());
    discounted_utils = {discounted_util, discounted_util};
    expected.min_util += 1.0;
    expected.mult_util += 1.0;
    expected.discount_min_util += discounted_util;
    expected.discount_mult_util += discounted_util;
    testPerson->AccumulateTotalUtility(undiscounted_utils, discounted_utils);
    EXPECT_EQ(expected, testPerson->GetTotalUtility());

    // more complex case
    testPerson->Grow();
    testPerson->SetUtility(0.8, utility::UtilityCategory::LIVER);
    testPerson->SetUtility(0.8, utility::UtilityCategory::BEHAVIOR);
    // { 0.8, 0.64}
    undiscounted_utils = testPerson->GetUtility();
    discounted_utils = {Utils::discount(undiscounted_utils.first, discount_rate,
                                        testPerson->GetCurrentTimestep()),
                        Utils::discount(undiscounted_utils.second,
                                        discount_rate,
                                        testPerson->GetCurrentTimestep())};
    expected.min_util += 0.8;
    expected.mult_util += 0.64;
    expected.discount_min_util += discounted_utils.first;
    expected.discount_mult_util += discounted_utils.second;
    testPerson->AccumulateTotalUtility(undiscounted_utils, discounted_utils);
    EXPECT_EQ(expected, testPerson->GetTotalUtility());
}

TEST_F(PersonTest, UndiscountedLifeSpan) {
    // test initialized state
    int expected = 0;
    EXPECT_EQ(expected, testPerson->GetLifeSpan());

    // test aging a few timesteps to check lifespan is correctly incremented
    testPerson->Grow();
    testPerson->Grow();
    testPerson->Grow();
    expected = 3;
    EXPECT_EQ(expected, testPerson->GetLifeSpan());
}

TEST_F(PersonTest, DiscountedLifeSpan) {
    // test initial state
    double expected = 0.0;
    EXPECT_EQ(expected, testPerson->GetDiscountedLifeSpan());

    double discount_rate = 0.025;
    int TIMESTEPS = 10;
    for (int i = 0; i < TIMESTEPS; ++i) {
        double discounted_life = Utils::discount(
            1.0, discount_rate, testPerson->GetCurrentTimestep());
        expected += discounted_life;
        testPerson->AddDiscountedLifeSpan(discounted_life);
        testPerson->Grow();
    }
    EXPECT_EQ(expected, testPerson->GetDiscountedLifeSpan());
}

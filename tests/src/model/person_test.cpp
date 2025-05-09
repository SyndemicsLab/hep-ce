////////////////////////////////////////////////////////////////////////////////
// File: person_test.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-05-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/model/person.hpp>

// STL Libraries
#include <filesystem>

// 3rd Party Dependencies
#include <datamanagement/datamanagement.hpp>
#include <gtest/gtest.h>

#include <hepce/data/types.hpp>
#include <hepce/utils/logging.hpp>

using namespace hepce::model;
using namespace hepce::data;

class PersonTest : public ::testing::Test {
protected:
    const std::string LOG_NAME = "PersonTest";
    std::unique_ptr<Person> person;
    static void SetUpTestSuite() {
        const std::string log_name = "PersonTest";
        const std::string log_file = log_name + ".log";
        hepce::utils::CreateFileLogger(log_name, log_file);
    }

    static void TearDownTestSuite() {
        std::filesystem::remove("PersonTest.log");
    }

    // Creating the Person object and growing to set the Current Time to 3
    void SetUp() override {
        person = Person::Create(LOG_NAME);
        person->Grow();
        person->Grow();
        person->Grow();
    }
    void TearDown() override {}
};

// Functionality Testing
TEST_F(PersonTest, Grow) {
    int b_time = person->GetCurrentTimestep();
    int b_age = person->GetAge();
    int b_ls = person->GetLifeSpan();

    person->Grow();

    EXPECT_EQ(person->GetCurrentTimestep(), b_time + 1);
    EXPECT_EQ(person->GetAge(), b_age + 1);
    EXPECT_EQ(person->GetLifeSpan(), b_ls + 1);
}

TEST_F(PersonTest, UpdateTimers) {
    person->SetBehavior(Behavior::kNoninjection);
    person->SetMoudState(MOUD::kCurrent);
    person->SetHIV(HIV::kLoUn);

    int b_behavior = person->GetBehaviorDetails().time_last_active;
    int b_moud = person->GetMoudDetails().total_moud_months;
    int b_hiv = person->GetHIVDetails().low_cd4_months_count;

    person->Grow();

    EXPECT_EQ(person->GetBehaviorDetails().time_last_active, b_behavior + 1);
    EXPECT_EQ(person->GetMoudDetails().total_moud_months, b_moud + 1);
    EXPECT_EQ(person->GetHIVDetails().low_cd4_months_count, b_hiv + 1);
}

TEST_F(PersonTest, Die) {
    person->Die(DeathReason::kAge);
    EXPECT_FALSE(person->IsAlive());
    EXPECT_EQ(person->GetDeathReason(), DeathReason::kAge);
}

TEST_F(PersonTest, SetPersonDetails) {
    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kNone);

    PersonSelect person_select;
    person_select.hcv = HCV::kChronic;
    person->SetPersonDetails(person_select);

    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kChronic);
}

// HCV Testing
TEST_F(PersonTest, InfectHCV_PriorInfection) {
    person->SetHCV(HCV::kChronic);
    person->InfectHCV();
    EXPECT_EQ(person->GetHCVDetails().times_infected, 0); // Default is 0
}

TEST_F(PersonTest, InfectHCV_NewInfection) {
    person->InfectHCV();
    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kAcute);
    EXPECT_EQ(person->GetHCVDetails().times_infected, 1);
    EXPECT_EQ(person->GetHCVDetails().fibrosis_state, FibrosisState::kF0);
}

TEST_F(PersonTest, ClearHCV_NotAcute) {
    person->SetHCV(HCV::kChronic);
    person->ClearHCV(false);

    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kNone);
    EXPECT_EQ(person->GetHCVDetails().times_acute_cleared, 0);
}

TEST_F(PersonTest, ClearHCV_Acute) {
    person->SetHCV(HCV::kAcute);
    person->ClearHCV(true);

    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kNone);
    EXPECT_EQ(person->GetHCVDetails().times_acute_cleared, 1);
}

TEST_F(PersonTest, Diagnose) {
    person->Diagnose(InfectionType::kHcv);
    EXPECT_TRUE(person->GetScreeningDetails(InfectionType::kHcv).identified);
    EXPECT_TRUE(person->GetScreeningDetails(InfectionType::kHcv).ab_positive);
}

TEST_F(PersonTest, ClearDiagnosis) {
    person->Diagnose(InfectionType::kHcv);
    person->ClearDiagnosis(InfectionType::kHcv);
    EXPECT_FALSE(person->GetScreeningDetails(InfectionType::kHcv).identified);
    EXPECT_TRUE(person->GetScreeningDetails(InfectionType::kHcv).ab_positive);
}

TEST_F(PersonTest, IsCirrhotic_False) {
    person->SetFibrosis(FibrosisState::kF1);
    EXPECT_FALSE(person->IsCirrhotic());
}

TEST_F(PersonTest, IsCirrhotic_F4) {
    person->SetFibrosis(FibrosisState::kF4);
    EXPECT_TRUE(person->IsCirrhotic());
}

TEST_F(PersonTest, IsCirrhotic_DECOMP) {
    person->SetFibrosis(FibrosisState::kDecomp);
    EXPECT_TRUE(person->IsCirrhotic());
}

// Screening Testing
TEST_F(PersonTest, Screen_Ab) {
    InfectionType hcv = InfectionType::kHcv;
    person->Screen(hcv, ScreeningTest::kAb, ScreeningType::kBackground);

    int time = person->GetCurrentTimestep();
    EXPECT_EQ(person->GetScreeningDetails(hcv).time_of_last_screening, time);
    EXPECT_EQ(person->GetScreeningDetails(hcv).num_ab_tests, 1);
}

TEST_F(PersonTest, AddRnaScreen) {
    InfectionType hcv = InfectionType::kHcv;
    person->Screen(hcv, ScreeningTest::kRna, ScreeningType::kBackground);

    int time = person->GetCurrentTimestep();
    EXPECT_EQ(person->GetScreeningDetails(hcv).time_of_last_screening, time);
    EXPECT_EQ(person->GetScreeningDetails(hcv).num_rna_tests, 1);
}

TEST_F(PersonTest, HadSecondScreening) {}

////////////////////////////////////////////////////////////////////////////////
// File: person_test.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/model/person.hpp>

// STL Includes
#include <memory>
#include <string>
#include <vector>

// 3rd Party Dependencies
#include <datamanagement/datamanagement.hpp>
#include <gtest/gtest.h>

// Library Headers
#include <hepce/utils/logging.hpp>
#include <hepce/utils/math.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Test Includes
#include <config.hpp>
#include <inputs_db.hpp>

namespace hepce {
namespace testing {

class PersonTest : public ::testing::Test {
protected:
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PersonTest, Grow) {
    const std::string LOG_NAME = "Grow";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    int age = person->GetAge();
    int lifespan = person->GetLifeSpan();
    int timestep = person->GetCurrentTimestep();
    int time_last_active = person->GetBehaviorDetails().time_last_active;
    int moud_current_state_count =
        person->GetMoudDetails().current_state_concurrent_months;

    person->Grow();

    ASSERT_EQ(person->GetAge(), age + 1);
    ASSERT_EQ(person->GetLifeSpan(), lifespan + 1);
    ASSERT_EQ(person->GetCurrentTimestep(), timestep + 1);
    ASSERT_EQ(person->GetBehaviorDetails().time_last_active, time_last_active);
    ASSERT_EQ(person->GetMoudDetails().current_state_concurrent_months,
              moud_current_state_count + 1);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(PersonTest, Die) {
    const std::string LOG_NAME = "Die";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    person->Die(data::DeathReason::kBackground);

    ASSERT_EQ(person->IsAlive(), false);
    ASSERT_EQ(person->GetDeathReason(), data::DeathReason::kBackground);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(PersonTest, InfectHCV_NoFibrosis) {
    const std::string LOG_NAME = "InfectHCV_NoFibrosis";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    int times_infected = person->GetHCVDetails().times_infected;

    person->InfectHCV();

    ASSERT_EQ(person->GetHCVDetails().hcv, data::HCV::kAcute);
    ASSERT_EQ(person->GetHCVDetails().time_changed,
              person->GetCurrentTimestep());
    ASSERT_EQ(person->GetHCVDetails().seropositive, true);
    ASSERT_EQ(person->GetHCVDetails().times_infected, times_infected + 1);
    ASSERT_EQ(person->GetHCVDetails().fibrosis_state, data::FibrosisState::kF0);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(PersonTest, InfectHCV_Fibrosis) {
    const std::string LOG_NAME = "InfectHCV_Fibrosis";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    int times_infected = person->GetHCVDetails().times_infected;
    person->UpdateTrueFibrosis(data::FibrosisState::kF4);

    person->InfectHCV();

    ASSERT_EQ(person->GetHCVDetails().hcv, data::HCV::kAcute);
    ASSERT_EQ(person->GetHCVDetails().time_changed,
              person->GetCurrentTimestep());
    ASSERT_EQ(person->GetHCVDetails().seropositive, true);
    ASSERT_EQ(person->GetHCVDetails().times_infected, times_infected + 1);
    ASSERT_EQ(person->GetHCVDetails().fibrosis_state, data::FibrosisState::kF4);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(PersonTest, InfectHCV_AlreadyChronic) {
    const std::string LOG_NAME = "InfectHCV_AlreadyChronic";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    int times_infected = person->GetHCVDetails().times_infected;
    person->UpdateTrueFibrosis(data::FibrosisState::kF1);
    person->SetHCV(data::HCV::kChronic);

    person->InfectHCV();

    ASSERT_NE(person->GetHCVDetails().hcv, data::HCV::kAcute);
    ASSERT_EQ(person->GetHCVDetails().times_infected, times_infected);
    ASSERT_EQ(person->GetHCVDetails().fibrosis_state, data::FibrosisState::kF1);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(PersonTest, ClearHCV_NotAcute) {
    const std::string LOG_NAME = "ClearHCV_NotAcute";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    person->SetHCV(data::HCV::kChronic);
    int acute_clearances = person->GetHCVDetails().times_acute_cleared;

    person->ClearHCV(false);

    ASSERT_EQ(person->GetHCVDetails().hcv, data::HCV::kNone);
    ASSERT_EQ(person->GetHCVDetails().time_changed,
              person->GetCurrentTimestep());
    ASSERT_EQ(person->GetHCVDetails().times_acute_cleared, acute_clearances);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(PersonTest, ClearHCV_Acute) {
    const std::string LOG_NAME = "ClearHCV_Acute";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    auto person = model::Person::Create(LOG_NAME);

    person->SetHCV(data::HCV::kChronic);
    int acute_clearances = person->GetHCVDetails().times_acute_cleared;

    person->ClearHCV(true);

    ASSERT_EQ(person->GetHCVDetails().hcv, data::HCV::kNone);
    ASSERT_EQ(person->GetHCVDetails().time_changed,
              person->GetCurrentTimestep());
    ASSERT_EQ(person->GetHCVDetails().times_acute_cleared,
              acute_clearances + 1);

    std::filesystem::remove(LOG_FILE);
}

// Functionality Tests

} // namespace testing
} // namespace hepce

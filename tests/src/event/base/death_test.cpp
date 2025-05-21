////////////////////////////////////////////////////////////////////////////////
// File: DeathTest.cpp                                                        //
// Project: hep-ce                                                            //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-15                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/base/death.hpp>

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
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

using namespace hepce::data;
using namespace hepce::event;

namespace hepce {
namespace testing {

class DeathTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    data::HCVDetails hcv_details = {data::HCV::kNone,
                                    data::FibrosisState::kNone,
                                    false,
                                    false,
                                    -1,
                                    -1,
                                    0,
                                    0,
                                    0};

    std::unique_ptr<datamanagement::ModelData> model_data;
    double discounted_cost;
    double discounted_life;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
    void SetUp() override {
        ExecuteQueries(test_db, {{"DROP TABLE IF EXISTS background_mortality;",
                                  "DROP TABLE IF EXISTS smr;",
                                  CreateBackgroundMortalities(), CreateSmrs(),
                                  "INSERT INTO background_mortality "
                                  "VALUES (25, 0, 0.001);",
                                  "INSERT INTO smr "
                                  "VALUES (0, 4, 6.1);"}});
        BuildSimConf(test_conf);
        discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
        discounted_life = utils::Discount(1, 0.0025, 1, false);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetCurrentlyOverdosing())
            .WillByDefault(Return(false));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behaviors));
    }
    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(DeathTest, AgeDR) {

    EXPECT_CALL(mock_person, GetAge()).WillOnce(Return(1200));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kAge)).Times(1);

    const std::string LOG_NAME = "AgeDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, F4_Infected_BackgroundDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kF4;
    hcv_details.hcv = data::HCV::kChronic;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kBackground)).Times(1);

    const std::string LOG_NAME = "F4_Infected_BackgroundDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, F4_Infected_LiverDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kF4;
    hcv_details.hcv = data::HCV::kChronic;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kLiver)).Times(1);

    const std::string LOG_NAME = "F4_Infected_LiverDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, F4_Uninfected_BackgroundDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kF4;
    hcv_details.hcv = data::HCV::kNone;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kBackground)).Times(1);

    const std::string LOG_NAME = "F4_Uninfected_BackgroundDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, F4_Uninfected_LiverDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kF4;
    hcv_details.hcv = data::HCV::kNone;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kLiver)).Times(1);

    const std::string LOG_NAME = "F4_Uninfected_LiverDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, Decomp_Infected_BackgroundDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kDecomp;
    hcv_details.hcv = data::HCV::kChronic;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kBackground)).Times(1);

    const std::string LOG_NAME = "Decomp_Infected_BackgroundDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, Decomp_Infected_LiverDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kDecomp;
    hcv_details.hcv = data::HCV::kChronic;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kLiver)).Times(1);

    const std::string LOG_NAME = "Decomp_Infected_LiverDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, Decomp_Uninfected_BackgroundDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kDecomp;
    hcv_details.hcv = data::HCV::kNone;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kBackground)).Times(1);

    const std::string LOG_NAME = "Decomp_Uninfected_BackgroundDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, Decomp_Uninfected_LiverDR) {
    hcv_details.fibrosis_state = data::FibrosisState::kDecomp;
    hcv_details.hcv = data::HCV::kNone;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Die(data::DeathReason::kLiver)).Times(1);

    const std::string LOG_NAME = "Decomp_Uninfected_LiverDR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(DeathTest, NoDeath) {
    hcv_details.fibrosis_state = data::FibrosisState::kDecomp;
    hcv_details.hcv = data::HCV::kNone;

    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv_details));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(mock_person, Die(_)).Times(0);

    const std::string LOG_NAME = "NoDeath";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // Running Test
    auto event = event::base::Death::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}
} // namespace testing
} // namespace hepce
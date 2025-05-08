////////////////////////////////////////////////////////////////////////////////
// File: staging_test.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/fibrosis/staging.hpp>

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
using ::testing::Return;

namespace hepce {
namespace testing {

class StagingTest : public ::testing::Test {
protected:
    MockPerson mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
    data::HCVDetails hcv = {data::HCV::kNone,
                            data::FibrosisState::kNone,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::StagingDetails staging = {data::MeasuredFibrosisState::kNone, false,
                                    -1};
    data::ScreeningDetails screen = {-1, 0, 0, false, false, -1};

    void SetUp() override {
        ExecuteQueries(test_db,
                       {{"DROP TABLE IF EXISTS fibrosis;", CreateFibrosis(),
                         "INSERT INTO fibrosis "
                         "VALUES (0, 0, 0.4, 0.9, 0.2, 1, 0.6);",
                         "INSERT INTO fibrosis "
                         "VALUES (0, 1, 0.4, 0.1, 0.6, 0, 0.3);"
                         "INSERT INTO fibrosis "
                         "VALUES (0, 2, 0.1, 0.0, 0.2, 0, 0.1);"
                         "INSERT INTO fibrosis "
                         "VALUES (0, 3, 0.0, 0.0, 0.0, 0, 0.0);",
                         "INSERT INTO fibrosis "
                         "VALUES (1, 0, 0.2, 0.1, 0.6, 0, 0.3);",
                         "INSERT INTO fibrosis "
                         "VALUES (1, 1, 0.4, 0.9, 0.2, 1, 0.6);"
                         "INSERT INTO fibrosis "
                         "VALUES (1, 2, 0.3, 0.0, 0.2, 0, 0.1);"
                         "INSERT INTO fibrosis "
                         "VALUES (1, 3, 0.0, 0.0, 0.0, 0, 0.0);"}});
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(StagingTest, NoFibrosis) {
    const std::string LOG_NAME = "NoFibrosis";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::fibrosis::Staging::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(StagingTest, StagingLimit) {
    const std::string LOG_NAME = "StagingLimit";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.fibrosis_state = data::FibrosisState::kF0;
    staging.time_of_last_staging = 3;

    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, GetFibrosisStagingDetails())
        .WillOnce(Return(staging));

    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(5));
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::fibrosis::Staging::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(StagingTest, NeverStaged_NoTestTwo) {
    const std::string LOG_NAME = "NeverStaged_NoTestTwo";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.fibrosis_state = data::FibrosisState::kF0;
    staging.time_of_last_staging = -1;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(3)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, GetFibrosisStagingDetails())
        .WillOnce(Return(staging));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(2));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                DiagnoseFibrosis(data::MeasuredFibrosisState::kF01))
        .Times(1);

    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kStaging))
        .Times(1);

    auto event = event::fibrosis::Staging::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(StagingTest, NeverStaged_TestTwo_latest) {
    const std::string LOG_NAME = "NeverStaged_TestTwo_latest";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.fibrosis_state = data::FibrosisState::kF1;
    staging.time_of_last_staging = -1;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(4)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, GetFibrosisStagingDetails())
        .WillOnce(Return(staging));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(3)
        .WillRepeatedly(Return(2));
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                DiagnoseFibrosis(data::MeasuredFibrosisState::kF23))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kStaging))
        .Times(1);

    EXPECT_CALL(mock_person, GiveSecondScreeningTest(true)).Times(1);
    EXPECT_CALL(mock_person,
                DiagnoseFibrosis(data::MeasuredFibrosisState::kF01))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(140, _, model::CostCategory::kStaging))
        .Times(1);

    auto event = event::fibrosis::Staging::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(StagingTest, NeverStaged_TestTwo_maximum) {
    const std::string LOG_NAME = "NeverStaged_TestTwo_maximum";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    BuildAlternateSimConf("sim2.conf");
    auto alt_model_data = datamanagement::ModelData::Create("sim2.conf");
    alt_model_data->AddSource(test_db);

    hcv.fibrosis_state = data::FibrosisState::kF1;
    staging.time_of_last_staging = -1;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(4)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, GetFibrosisStagingDetails())
        .WillOnce(Return(staging));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(3)
        .WillRepeatedly(Return(2));
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                DiagnoseFibrosis(data::MeasuredFibrosisState::kF23))
        .Times(2);
    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kStaging))
        .Times(1);

    EXPECT_CALL(mock_person, GiveSecondScreeningTest(true)).Times(1);
    EXPECT_CALL(mock_person, AddCost(140, _, model::CostCategory::kStaging))
        .Times(1);

    auto event = event::fibrosis::Staging::Create(*alt_model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
    std::filesystem::remove("sim2.conf");
}

} // namespace testing
} // namespace hepce

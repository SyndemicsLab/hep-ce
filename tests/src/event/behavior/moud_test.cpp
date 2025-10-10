////////////////////////////////////////////////////////////////////////////////
// File: moud_test.cpp                                                        //
// Project: hep-ce                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/behavior/moud.hpp>

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
#include <utility.hpp>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class MoudTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    data::HCVDetails hcv = {data::HCV::kNone,
                            data::FibrosisState::kF0,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::ScreeningDetails screen = {-1, 0, 0, false, false, -1, 0};

    void SetUp() override {
        ExecuteQueries(test_db, {{"DROP TABLE IF EXISTS moud_transitions;",
                                  CreateMoudTransitions(),
                                  "INSERT INTO moud_transitions VALUES (25, 1, "
                                  "4, -1, 0.0, 1.0, 0.0);"
                                  "INSERT INTO moud_transitions VALUES (25, 1, "
                                  "8, -1, 0.0, 0.0, 1.0);",
                                  "INSERT INTO moud_transitions VALUES (25, 0, "
                                  "0, -1, 1.0, 0.0, 0.0);",
                                  "INSERT INTO moud_transitions VALUES (30, 0, "
                                  "0, -1, 0.0, 1.0, 0.0);"}});
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(MoudTest, PersonIsDead) {
    // Setup
    const std::string LOG_NAME = "PersonIsDead";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current;

    // Expectations
    EXPECT_CALL(mock_person, IsAlive()).WillOnce(Return(false));
    EXPECT_CALL(mock_person, GetBehaviorDetails()).Times(0);
    EXPECT_CALL(mock_person, GetMoudDetails()).Times(0);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(MoudTest, NoHistoryOfOUD) {
    // Setup
    const std::string LOG_NAME = "NoHistoryOfOUD";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current;

    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));

    // Expectations
    EXPECT_CALL(mock_person, GetMoudDetails()).Times(0);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(MoudTest, PostTreatment) {
    // Setup
    const std::string LOG_NAME = "PostTreatment";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud_details = {data::MOUD::kPost, -1, 1, 1};

    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud_details));

    // Expectations
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(1);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(MoudTest, StayOnTreatment) {
    // Setup
    const std::string LOG_NAME = "StayOnTreatment";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud_details = {data::MOUD::kCurrent, 0, 4, 4};

    std::vector<double> expected_probs = {0.0, 1.0, 0.0};

    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud_details));
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(expected_probs)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(0);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(MoudTest, StopTreatment) {
    // Setup
    const std::string LOG_NAME = "StopTreatment";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud_details = {data::MOUD::kCurrent, 0, 8, 8};

    std::vector<double> expected_probs = {0.0, 0.0, 1.0};

    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud_details));
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(expected_probs)).WillOnce(Return(2));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(1);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(MoudTest, StartTreatment) {
    // Setup
    const std::string LOG_NAME = "StartTreatment";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud_details = {data::MOUD::kNone, 0, 0, 0};

    std::vector<double> expected_probs = {0.0, 1.0, 0.0};

    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud_details));
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(360));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(expected_probs)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(1);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(MoudTest, StayOffTreatment) {
    // Setup
    const std::string LOG_NAME = "StayOffTreatment";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behavior_current = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud_details = {data::MOUD::kNone, 0, 4, 2};

    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud_details));
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(0);

    // Run test
    auto event = event::behavior::Moud::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

} // namespace testing
} // namespace hepce

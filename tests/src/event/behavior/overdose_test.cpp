////////////////////////////////////////////////////////////////////////////////
// File: overdose_test.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/behavior/overdose.hpp>

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

class OverdoseTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    data::BehaviorDetails behavior_current;
    data::MOUDDetails moud_details;
    data::PregnancyDetails pregnancy_details;

    void SetUp() override {
        ExecuteQueries(
            test_db,
            {{"DROP TABLE IF EXISTS overdoses;",
              CreateOverdoses(),
              "INSERT INTO overdoses VALUES (-1, 0, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (-1, 0, 4, 0.5, 10.00, 0.2);",
              "INSERT INTO overdoses VALUES (-1, 1, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (-1, 1, 4, 0.3, 10.00, 0.2);",
              "INSERT INTO overdoses VALUES (-1, 2, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (-1, 2, 4, 0.6, 10.00, 0.2);",
              "INSERT INTO overdoses VALUES (0, 0, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (0, 0, 4, 0.5, 10.00, 0.2);",
              "INSERT INTO overdoses VALUES (0, 1, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (0, 1, 4, 0.3, 10.00, 0.2);",
              "INSERT INTO overdoses VALUES (0, 2, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (0, 2, 4, 0.6, 10.00, 0.2);",
              "INSERT INTO overdoses VALUES (1, 0, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (1, 0, 4, 0.5, 12.00, 0.1);",
              "INSERT INTO overdoses VALUES (1, 1, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (1, 1, 4, 0.3, 12.00, 0.1);",
              "INSERT INTO overdoses VALUES (1, 2, 1, 0.0, 0.0, 1.0);",
              "INSERT INTO overdoses VALUES (1, 2, 4, 0.6, 12.00, 0.1);"}});
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior_current));
        ON_CALL(mock_person, GetMoudDetails())
            .WillByDefault(Return(moud_details));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy_details));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(OverdoseTest, PersonIsDead) {
    // Setup
    const std::string LOG_NAME = "PersonIsDead";
    CreateTestLog(LOG_NAME);

    // Expectations
    EXPECT_CALL(mock_person, IsAlive()).WillOnce(Return(false));
    EXPECT_CALL(mock_person, GetPregnancyDetails()).Times(0);
    EXPECT_CALL(mock_person, GetMoudDetails()).Times(0);

    // Run test
    auto event = event::behavior::Overdose::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(OverdoseTest, OverdoseOccurs) {
    // Setup
    const std::string LOG_NAME = "OverdoseOccurs";
    CreateTestLog(LOG_NAME);

    // Adjust On Call Returns
    behavior_current.behavior = data::Behavior::kInjection;
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));

    // Expectations
    std::vector<double> prob = {0.5, 0.5};
    EXPECT_CALL(mock_sampler, GetDecision(prob)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, ToggleOverdose()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(1);

    // Run test
    auto event = event::behavior::Overdose::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(OverdoseTest, NoOverdoseOccurs) {
    // Setup
    const std::string LOG_NAME = "NoOverdoseOccurs";
    CreateTestLog(LOG_NAME);

    // Adjust On Call Returns
    behavior_current.behavior = data::Behavior::kFormerNoninjection;
    ON_CALL(mock_person, GetBehaviorDetails())
        .WillByDefault(Return(behavior_current));

    // Expectations
    std::vector<double> prob = {0.0, 1.0};
    EXPECT_CALL(mock_sampler, GetDecision(prob)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, ToggleOverdose()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    // Run test
    auto event = event::behavior::Overdose::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}
} // namespace testing
} // namespace hepce

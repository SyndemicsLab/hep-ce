////////////////////////////////////////////////////////////////////////////////
// File: BehaviorChangesTest.cpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/behavior/behavior_changes.hpp>

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

class BehaviorChangesTest : public ::testing::Test {
protected:
    MockPerson mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    double discounted_cost;
    double discounted_life;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
    data::MOUDDetails mouds = {data::MOUD::kNone, -1, 0, 0};

    void SetUp() override {
        ExecuteQueries(
            test_db, {{"DROP TABLE IF EXISTS behavior_transitions;",
                       "DROP TABLE IF EXISTS behavior_impacts;",
                       CreateBehaviorImpacts(), CreateBehaviorTransitions(),
                       "INSERT INTO behavior_impacts "
                       "VALUES (0, 4, 370.75, 0.574);",
                       "INSERT INTO behavior_impacts "
                       "VALUES (0, 2, 86.24, 0.744);"
                       "INSERT INTO behavior_transitions "
                       "VALUES (25, 0, 4, 0, 0.0, 0.0, 0.002, 0.0, 0.998);"}});
        BuildSimConf(test_conf);
        discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
        discounted_life = utils::Discount(1, 0.0025, 1, false);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(BehaviorChangesTest, InjectionToFormer) {
    data::BehaviorDetails behaviors_new = {data::Behavior::kFormerInjection, 0};

    EXPECT_CALL(mock_person, GetAge()).Times(1).WillRepeatedly(Return(300));
    EXPECT_CALL(mock_person, GetSex())
        .Times(2)
        .WillRepeatedly(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails())
        .WillOnce(Return(behaviors))
        .WillOnce(Return(behaviors_new));
    EXPECT_CALL(mock_person, GetMoudDetails())
        .Times(1)
        .WillRepeatedly(Return(mouds));
    // Return former injection behavior
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kFormerInjection))
        .Times(1);

    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, AddCost(86.24, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.744, model::UtilityCategory::kBehavior))
        .Times(1);

    const std::string LOG_NAME = "InjectionToFormer";
    const std::string LOG_FILE = "InjectionToFormer.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(BehaviorChangesTest, InvalidPersonDetails) {
    EXPECT_CALL(mock_person, GetAge()).Times(1).WillRepeatedly(Return(300));
    EXPECT_CALL(mock_person, GetSex())
        .Times(3)
        .WillRepeatedly(Return(data::Sex::kFemale));
    EXPECT_CALL(mock_person, GetBehaviorDetails())
        .Times(3)
        .WillRepeatedly(Return(behaviors));
    EXPECT_CALL(mock_person, GetMoudDetails())
        .Times(1)
        .WillRepeatedly(Return(mouds));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);

    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0, model::UtilityCategory::kBehavior))
        .Times(1);

    const std::string LOG_NAME = "InvalidPersonDetails";
    const std::string LOG_FILE = "InvalidPersonDetails.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::string expected =
        "Behavior Transition Probabilities are not found for the person "
        "details (age, Sex, Behavior, MOUD):";
    std::string line;

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(BehaviorChangesTest, InvalidSampling) {
    data::BehaviorDetails behaviors_new = {data::Behavior::kFormerInjection, 0};

    EXPECT_CALL(mock_person, GetAge()).Times(1).WillRepeatedly(Return(300));
    EXPECT_CALL(mock_person, GetSex())
        .Times(1)
        .WillRepeatedly(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails()).WillOnce(Return(behaviors));
    EXPECT_CALL(mock_person, GetMoudDetails())
        .Times(1)
        .WillRepeatedly(Return(mouds));
    // Invalid Sample Returned
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(5));

    EXPECT_CALL(mock_person, SetBehavior(_)).Times(0);

    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    const std::string LOG_NAME = "InvalidSampling";
    const std::string LOG_FILE = "InvalidSampling.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::string expected =
        "Invalid Decision returned during the Behavior Change Event!";
    std::string line;

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(BehaviorChangesTest, InvalidModelData) {
    const std::string LOG_NAME = "InvalidModelData";
    const std::string LOG_FILE = "InvalidModelData.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    std::unique_ptr<datamanagement::ModelData> empty_model_data =
        datamanagement::ModelData::Create(test_conf, LOG_NAME);

    EXPECT_CALL(mock_person, GetAge()).Times(1).WillRepeatedly(Return(300));
    EXPECT_CALL(mock_person, GetSex())
        .Times(3)
        .WillRepeatedly(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails())
        .Times(3)
        .WillRepeatedly(Return(behaviors));
    EXPECT_CALL(mock_person, GetMoudDetails())
        .Times(1)
        .WillRepeatedly(Return(mouds));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);

    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0, model::UtilityCategory::kBehavior))
        .Times(1);

    auto event =
        event::behavior::BehaviorChanges::Create(*empty_model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::string expected = "Error getting cost data in behavior changes:";
    std::string line;

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}
} // namespace testing
} // namespace hepce

////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes_test.cpp                                            //
// Project: hep-ce                                                            //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-31                                                  //
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
#include <utility.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::DoubleNear;
using ::testing::ElementsAre;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SaveArg;

namespace hepce {
namespace testing {

class BehaviorChangesTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    double discounted_cost;
    double discounted_life;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
    data::MOUDDetails mouds = {data::MOUD::kNone, -1, 0, 0};

    void SetUp() override {
        ExecuteQueries(test_db,
                       {{"DROP TABLE IF EXISTS behavior_transitions;",
                         "DROP TABLE IF EXISTS behavior_impacts;",
                         CreateBehaviorImpacts(), CreateBehaviorTransitions(),
                         "INSERT INTO behavior_impacts "
                         "VALUES (0, 4, 370.75, 0.574);",
                         "INSERT INTO behavior_impacts "
                         "VALUES (0, 2, 86.24, 0.744);"
                         "INSERT INTO behavior_transitions "
                         "VALUES (25, 0, 4, 0, 0.0, 0.0, 0.002, 0.0, 0.998);"
                         "INSERT INTO behavior_transitions "
                         "VALUES (25, 0, 2, 0, 0.0, 0.0, 0.5, 0.0, 0.5);"
                         "INSERT INTO behavior_transitions "
                         "VALUES (25, 0, 1, 0, 0.0, 0.5, 0.0, 0.5, 0.0);"
                         "INSERT INTO behavior_transitions "
                         "VALUES (30, 0, 1, 0, 0.0, 0.6, 0.0, 0.2, 0.2);"}});
        BuildSimConf(test_conf);
        discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
        discounted_life = utils::Discount(1, 0.0025, 1, false);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behaviors));
        ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(mouds));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(BehaviorChangesTest, InjectionToFormer) {
    // Setup
    const std::string LOG_NAME = "InjectionToFormer";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behaviors_new = {data::Behavior::kFormerInjection, 1};
    std::vector<double> expected_probs = {0.0, 0.0, 0.002, 0.0, 0.998};

    // Expectations
    EXPECT_CALL(mock_person, GetBehaviorDetails())
        .WillOnce(Return(behaviors))
        .WillOnce(Return(behaviors))
        .WillOnce(Return(behaviors_new));

    // Return former injection behavior
    EXPECT_CALL(mock_sampler, GetDecision(expected_probs)).WillOnce(Return(2));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kFormerInjection))
        .Times(1);

    EXPECT_CALL(mock_person, AddCost(86.24, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.744, model::UtilityCategory::kBehavior))
        .Times(1);

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, InvalidPersonDetails) {
    // Setup
    const std::string LOG_NAME = "InvalidPersonDetails";
    CreateTestLog(LOG_NAME);

    // Expectations
    EXPECT_CALL(mock_person, GetSex())
        .Times(3)
        .WillRepeatedly(Return(data::Sex::kFemale));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);

    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0, model::UtilityCategory::kBehavior))
        .Times(1);

    std::string expected =
        "Behavior Transition Probabilities are not found for the person "
        "details (age, Sex, Behavior, MOUD):";
    std::string line;

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::ifstream f(LOG_NAME + ".log");
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, InvalidSampling) {
    // Setup
    const std::string LOG_NAME = "InvalidSampling";
    CreateTestLog(LOG_NAME);
    data::BehaviorDetails behaviors_new = {data::Behavior::kFormerInjection, 0};

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(5));

    EXPECT_CALL(mock_person, SetBehavior(_)).Times(0);

    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    const std::string LOG_FILE = "InvalidSampling.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    std::string expected =
        "Invalid Decision returned during the Behavior Change Event!";
    std::string line;

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::ifstream f(LOG_NAME + ".log");
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, InvalidModelData) {
    // Setup
    const std::string LOG_NAME = "InvalidModelData";
    CreateTestLog(LOG_NAME);

    std::unique_ptr<datamanagement::ModelData> empty_model_data =
        datamanagement::ModelData::Create(test_conf, LOG_NAME);

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);

    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0, model::UtilityCategory::kBehavior))
        .Times(1);

    std::string expected = "Error getting cost data in behavior changes:";
    std::string line;

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*empty_model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::ifstream f(LOG_NAME + ".log");
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, RelapseToInjection) {
    // Setup
    const std::string LOG_NAME = "RelapseToInjection";
    CreateTestLog(LOG_NAME);

    behaviors = {data::Behavior::kFormerInjection, 0};
    std::vector<double> actual_probs;

    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behaviors));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(DoAll(SaveArg<0>(&actual_probs), Return(4)));

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    EXPECT_THAT(actual_probs, ElementsAre(0.0, 0.0, DoubleNear(0.5984, 1e-4),
                                          0.0, DoubleNear(0.4016, 1e-4)));

    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, RelapseToNoninjection) {
    // Setup
    const std::string LOG_NAME = "RelapseToNoninjection";
    CreateTestLog(LOG_NAME);

    behaviors = {data::Behavior::kFormerNoninjection, 0};
    std::vector<double> actual_probs;

    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behaviors));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(DoAll(SaveArg<0>(&actual_probs), Return(3)));

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    EXPECT_THAT(actual_probs, ElementsAre(0.0, DoubleNear(0.5984, 1e-4), 0.0,
                                          DoubleNear(0.4016, 1e-4), 0.0));

    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, LongTermRelapseToNoninjection) {
    // Setup
    const std::string LOG_NAME = "LongTermRelapseToNoninjection";
    CreateTestLog(LOG_NAME);

    behaviors = {data::Behavior::kFormerNoninjection, 0};
    std::vector<double> actual_probs;

    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(13));
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behaviors));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(DoAll(SaveArg<0>(&actual_probs), Return(1)));

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    EXPECT_THAT(actual_probs, ElementsAre(0.0, DoubleNear(0.9771, 1e-4), 0.0,
                                          DoubleNear(0.0228, 1e-4), 0.0));

    RemoveTestLog(LOG_NAME);
}

TEST_F(BehaviorChangesTest, RiskOfRelapseAndEscalation) {
    // Setup
    const std::string LOG_NAME = "RiskOfRelapseAndEscalation";
    CreateTestLog(LOG_NAME);

    behaviors = {data::Behavior::kFormerNoninjection, 0};
    std::vector<double> actual_probs;

    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behaviors));
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(360));

    // Expectations
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(DoAll(SaveArg<0>(&actual_probs), Return(1)));

    // Run test
    auto event =
        event::behavior::BehaviorChanges::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    EXPECT_THAT(actual_probs, ElementsAre(0.0, DoubleNear(0.6476, 1e-4), 0.0,
                                          DoubleNear(0.1523, 1e-4), 0.2));

    RemoveTestLog(LOG_NAME);
}
} // namespace testing
} // namespace hepce

////////////////////////////////////////////////////////////////////////////////
// File: progression_test.cpp                                                 //
// Project: hep-ce                                                            //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/fibrosis/progression.hpp>

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

namespace hepce {
namespace testing {

class ProgressionTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    double discounted_cost;
    double discounted_life;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
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
        ExecuteQueries(
            test_db, {{"DROP TABLE IF EXISTS hcv_impacts;", CreateHCVImpacts(),
                       "INSERT INTO hcv_impacts "
                       "VALUES (0, 0, 230.65, 0.915);",
                       "INSERT INTO hcv_impacts "
                       "VALUES (1, 0, 430.00, 0.8);",
                       "INSERT INTO hcv_impacts "
                       "VALUES (1, 1, 500.00, 0.7);"}});
        BuildSimConf(test_conf);
        discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
        discounted_life = utils::Discount(1, 0.0025, 1, false);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(ProgressionTest, NoHCV) {
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::fibrosis::Progression::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, NoProgression) {
    const std::string LOG_NAME = "NoProgression";
    const std::string LOG_FILE = "NoProgression.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    // person setup
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    // expectations
    EXPECT_CALL(mock_sampler, GetDecision({{0.008877, 1 - 0.008877}}))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCost(430.00, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0.8, model::UtilityCategory::kLiver))
        .Times(1);

    auto event = event::fibrosis::Progression::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(ProgressionTest, NoProgression_AddCostFlag_NoID) {
    const std::string LOG_NAME = "NoProgression_AddCost";
    const std::string LOG_FILE = "NoProgression_AddCost.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    auto config = DEFAULT_CONFIG;
    config["fibrosis"][5] = "add_cost_only_if_identified = true";
    BuildSimConf(test_conf, config);
    discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
    discounted_life = utils::Discount(1, 0.0025, 1, false);
    std::unique_ptr<datamanagement::ModelData> alt_model_data =
        datamanagement::ModelData::Create("sim.conf", LOG_NAME);
    alt_model_data->AddSource(test_db);

    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screen));

    // expectations
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(0.8, model::UtilityCategory::kLiver))
        .Times(1);

    auto event =
        event::fibrosis::Progression::Create(*alt_model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove("sim.conf");
    std::filesystem::remove(LOG_FILE);
}

TEST_F(ProgressionTest, NoProgression_AddCostFlag_ID) {
    const std::string LOG_NAME = "NoProgression_AddCost";
    const std::string LOG_FILE = "NoProgression_AddCost.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    auto config = DEFAULT_CONFIG;
    config["fibrosis"][5] = "add_cost_only_if_identified = true";
    BuildSimConf(test_conf, config);
    discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
    discounted_life = utils::Discount(1, 0.0025, 1, false);
    std::unique_ptr<datamanagement::ModelData> alt_model_data =
        datamanagement::ModelData::Create("sim.conf", LOG_NAME);
    alt_model_data->AddSource(test_db);

    hcv.hcv = data::HCV::kAcute;
    screen.identified = true;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screen));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCost(430.00, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0.8, model::UtilityCategory::kLiver))
        .Times(1);

    auto event =
        event::fibrosis::Progression::Create(*alt_model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove("sim.conf");
    std::filesystem::remove(LOG_FILE);
}

TEST_F(ProgressionTest, Progression) {
    const std::string LOG_NAME = "Progression";
    const std::string LOG_FILE = "Progression.log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kAcute;
    data::HCVDetails new_hcv = hcv;
    new_hcv.fibrosis_state = data::FibrosisState::kF1;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .WillOnce(Return(hcv))
        .WillOnce(Return(hcv))
        .WillOnce(Return(hcv))
        .WillOnce(Return(new_hcv))
        .WillOnce(Return(new_hcv))
        .WillOnce(Return(new_hcv))
        .WillOnce(Return(new_hcv))
        .WillOnce(Return(new_hcv));
    EXPECT_CALL(mock_sampler, GetDecision({{0.008877, 1 - 0.008877}}))
        .Times(1)
        .WillRepeatedly(Return(0));
    EXPECT_CALL(mock_person, SetFibrosis(data::FibrosisState::kF1)).Times(1);
    EXPECT_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .Times(0);

    EXPECT_CALL(mock_person, AddCost(500.00, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0.7, model::UtilityCategory::kLiver))
        .Times(1);

    auto event = event::fibrosis::Progression::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

} // namespace testing
} // namespace hepce

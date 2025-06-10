////////////////////////////////////////////////////////////////////////////////
// File: treatment_test.cpp                                                   //
// Project: hep-ce                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/hcv/treatment.hpp>

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

class HCVTreatmentTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
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
    data::LinkageDetails linkage = {data::LinkageState::kLinked, -1, 0};
    data::ScreeningDetails screen = {
        -1, 0, 0, false, false, -1, 0, data::ScreeningType::kIntervention};
    data::TreatmentDetails treatment = {false, 0, 0, 0, 0, 0, 0, false};

    void SetUp() override {
        ExecuteQueries(
            test_db, {{"DROP TABLE IF EXISTS treatments;", CreateTreatments(),
                       "INSERT INTO treatments "
                       "VALUES (0, 0, 0, \"gp\", 2, 10, 1, 0, 0.004, 0);",
                       "INSERT INTO treatments "
                       "VALUES (0, 0, 1, \"gp\", 2, 10, 1, 0, 0.004, 0);",
                       "INSERT INTO treatments "
                       "VALUES (0, 1, 0, \"gp\", 2, 10, .9, 0, 0.004, 0);",
                       "INSERT INTO treatments "
                       "VALUES (0, 1, 1, \"gp\", 2, 10, .8, .5, 0.004, .5);",
                       "INSERT INTO treatments "
                       "VALUES (1, 0, 1, \"svv\", 3, 20, 1, 0, 0.004, 0);",
                       "INSERT INTO treatments "
                       "VALUES (1, 0, 0, \"svv\", 3, 20, .9, 0, 0.004, 0);",
                       "INSERT INTO treatments "
                       "VALUES (1, 1, 1, \"svv\", 3, 20, .8, 0, 0.004, 0);",
                       "INSERT INTO treatments "
                       "VALUES (1, 1, 0,  \"svv\", 3, 20, 1, 0, 0.004, 0);"}});
        ExecuteQueries(test_db, {{"DROP TABLE IF EXISTS treatment_initiations;",
                                  CreateTreatmentInitializations(),
                                  "INSERT INTO treatment_initiations "
                                  "VALUES (-1, 0.92);",
                                  "INSERT INTO treatment_initiations "
                                  "VALUES (0, 0.92);",
                                  "INSERT INTO treatment_initiations "
                                  "VALUES (1, 0.92);",
                                  "INSERT INTO treatment_initiations "
                                  "VALUES (2, 0.92);",
                                  "INSERT INTO treatment_initiations "
                                  "VALUES (3, 0.92);",
                                  "INSERT INTO treatment_initiations "
                                  "VALUES (4, 0.92);"}});
        ExecuteQueries(test_db, {{"DROP TABLE IF EXISTS lost_to_follow_up;",
                                  CreateLostToFollowUps(),
                                  "INSERT INTO lost_to_follow_up "
                                  "VALUES (-1, 0.0);",
                                  "INSERT INTO lost_to_follow_up "
                                  "VALUES (0, 0.0);",
                                  "INSERT INTO lost_to_follow_up "
                                  "VALUES (1, 0.0);",
                                  "INSERT INTO lost_to_follow_up "
                                  "VALUES (2, 0.0);",
                                  "INSERT INTO lost_to_follow_up "
                                  "VALUES (3, 0.0);",
                                  "INSERT INTO lost_to_follow_up "
                                  "VALUES (4, 0.0);"}});
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetLinkageDetails(_))
            .WillByDefault(Return(linkage));
        ON_CALL(mock_person, GetTreatmentDetails(_))
            .WillByDefault(Return(treatment));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVTreatmentTest, NotLinked) {
    const std::string LOG_NAME = "NotLinked";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    EXPECT_CALL(mock_person, GetLinkageDetails(_)).WillOnce(Return(linkage));
    EXPECT_CALL(mock_person, Unlink(_)).Times(0);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, LostToFollowUp) {
    const std::string LOG_NAME = "LostToFollowUp";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, EndTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, Unlink(_)).Times(1);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, FirstTreatment) {
    const std::string LOG_NAME = "FirstTreatment";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, InitiateTreatment(_)).Times(1);

    EXPECT_CALL(mock_person,
                AddCost(12603.02, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(10, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, Salvage) {
    const std::string LOG_NAME = "Salvage";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    treatment.num_completed = 1;
    treatment.in_salvage_treatment = true;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, InitiateTreatment(_)).Times(1);

    EXPECT_CALL(mock_person,
                AddCost(19332.50, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(20, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, SVR) {
    const std::string LOG_NAME = "SVR";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    treatment.initiated_treatment = true;
    treatment.time_of_treatment_initiation = -1;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));

    EXPECT_CALL(mock_person,
                AddCost(12603.02, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(10, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCompletedTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, AddSVR()).Times(1);
    EXPECT_CALL(mock_person, ClearHCV(_)).Times(1);
    EXPECT_CALL(mock_person, ClearDiagnosis(_)).Times(1);
    EXPECT_CALL(mock_person, EndTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, Unlink(_)).Times(1);
    EXPECT_CALL(mock_person, SetUtility(1, model::UtilityCategory::kTreatment))
        .Times(1);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, StartSalvage) {
    const std::string LOG_NAME = "StartSalvage";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    treatment.initiated_treatment = true;
    treatment.time_of_treatment_initiation = -1;
    treatment.in_salvage_treatment = false;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));

    EXPECT_CALL(mock_person,
                AddCost(12603.02, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(10, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCompletedTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, InitiateTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, AddSVR()).Times(0);
    EXPECT_CALL(mock_person, ClearHCV(_)).Times(0);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, FailSalvage) {
    const std::string LOG_NAME = "FailSalvage";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    treatment.initiated_treatment = true;
    // Not possible, but GetCurrentTimestep returns 1 so set the diff to 3
    treatment.time_of_treatment_initiation = -2;
    treatment.in_salvage_treatment = true;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));

    EXPECT_CALL(mock_person,
                AddCost(19332.50, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(20, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCompletedTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, AddSVR()).Times(0);
    EXPECT_CALL(mock_person, ClearHCV(_)).Times(0);
    EXPECT_CALL(mock_person, EndTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, Unlink(_)).Times(1);
    EXPECT_CALL(mock_person, SetUtility(1, model::UtilityCategory::kTreatment))
        .Times(1);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVTreatmentTest, Withdraw) {
    const std::string LOG_NAME = "Withdraw";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    treatment.initiated_treatment = true;
    treatment.time_of_treatment_initiation = -1;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0));

    EXPECT_CALL(mock_person,
                AddCost(12603.02, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(10, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddWithdrawal(_)).Times(1);
    EXPECT_CALL(mock_person, EndTreatment(_)).Times(1);
    EXPECT_CALL(mock_person, Unlink(_)).Times(1);
    EXPECT_CALL(mock_person, SetUtility(1, model::UtilityCategory::kTreatment))
        .Times(1);

    EXPECT_CALL(mock_person, AddCompletedTreatment(_)).Times(0);

    auto event = event::hcv::Treatment::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

} // namespace testing
} // namespace hepce

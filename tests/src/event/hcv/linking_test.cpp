////////////////////////////////////////////////////////////////////////////////
// File: linking_test.cpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-05                                                  //
// Modified By: Andrew Clark                                                  //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/hcv/linking.hpp>

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

class HCVLinkingTest : public ::testing::Test {
protected:
    data::InfectionType TYPE = data::InfectionType::kHcv;
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unordered_map<std::string, std::vector<std::string>> config;
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
    data::LinkageDetails linkage = {data::LinkageState::kNever, -1, 0};
    data::ScreeningDetails screen = {-1,    0,  0, false,
                                     false, -1, 0, data::ScreeningType::kNa};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};

    void SetUp() override {
        ExecuteQueries(test_db, {{"DROP TABLE IF EXISTS screening_and_linkage;",
                                  CreateScreeningAndLinkage(),
                                  "INSERT INTO screening_and_linkage "
                                  "VALUES (25, 1, 4, 1, 0, 0.03, 1, 0.4);"}});
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex())
            .WillByDefault(Return(data::Sex::kFemale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behaviors));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVLinkingTest, Linked) {
    const std::string LOG_NAME = "Linked";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kLinked;
    screen.identified = true;

    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(_)).WillByDefault(Return(screen));

    // Ensure we don't execute anything if we are already linked.
    EXPECT_CALL(mock_person, Link(_)).Times(0);
    EXPECT_CALL(mock_person, ClearDiagnosis(_)).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, NotIdentified) {
    const std::string LOG_NAME = "NotIdentified";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kBackground;
    screen.identified = false;

    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(_)).WillByDefault(Return(screen));

    // Ensure we don't progress and try to evaluate details if they're not identified
    EXPECT_CALL(mock_person, Link(_)).Times(0);
    EXPECT_CALL(mock_person, ClearDiagnosis(_)).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, FalsePositive) {
    const std::string LOG_NAME = "FalsePositive";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    screen.identified = true;
    hcv.hcv = data::HCV::kNone;

    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(_)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    // Clear and add costs automatically, Not Sample Dependent
    EXPECT_CALL(mock_person, FalsePositive(TYPE)).Times(1);
    EXPECT_CALL(mock_person, AddCost(442.39, _, model::CostCategory::kLinking))
        .Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, RecentScreen) {
    const std::string LOG_NAME = "RecentScreen";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kIntervention;
    screen.identified = true;
    screen.time_of_last_screening = 1;
    hcv.hcv = data::HCV::kAcute;

    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(_)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    double v = utils::RateToProbability(utils::ProbabilityToRate(0.4) * 1.1);
    std::vector<double> p = {v};

    // Test the Decision But do not clear
    EXPECT_CALL(mock_sampler, GetDecision(p)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, ClearDiagnosis(_)).Times(0);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, BackgroundLink) {
    const std::string LOG_NAME = "BackgroundLink";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kBackground;
    screen.identified = true;
    screen.time_of_last_screening = 1;
    hcv.hcv = data::HCV::kAcute;

    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(_)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(7));

    // Link and Add the Respective Costs. Should Require 1 Sample
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(_)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, InterventionLink) {
    const std::string LOG_NAME = "InterventionLink";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kIntervention;
    screen.identified = true;
    screen.time_of_last_screening = 1;
    hcv.hcv = data::HCV::kAcute;

    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(_)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(7));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(7));

    // Ensure we link and add the costs. Only requires 1 Sampling
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(_)).Times(1);
    EXPECT_CALL(mock_person, AddCost(0, _, model::CostCategory::kLinking))
        .Times(1);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, LinkRateExpDecay) {
    const std::string LOG_NAME = "LinkRateExpDecay";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    config = {{"linking",
               {"recent_screen_cutoff = 10", "scaling_coefficient = 1.0",
                "scaling_type = exponential"}}};

    BuildSimConf(test_conf, config);
    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    model_data->AddSource(test_db);

    auto it = data::InfectionType::kHcv;
    hcv.hcv = data::HCV::kAcute;
    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kBackground;
    screen.identified = true;

    ON_CALL(mock_person, GetLinkageDetails(it)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(it)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(5));

    EXPECT_CALL(mock_sampler, GetDecision({{7.5497976736560446e-5}}))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(it)).Times(1);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, LinkRateMultiplier) {
    const std::string LOG_NAME = "LinkRateMultiplier";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    config = {{"linking",
               {"recent_screen_cutoff = 10", "scaling_coefficient = 0.5",
                "scaling_type = multiplier"}}};

    BuildSimConf(test_conf, config);
    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    model_data->AddSource(test_db);

    auto it = data::InfectionType::kHcv;
    hcv.hcv = data::HCV::kAcute;
    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kBackground;
    screen.identified = true;

    ON_CALL(mock_person, GetLinkageDetails(it)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(it)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(5));

    EXPECT_CALL(mock_sampler, GetDecision({{0.015114219820389518}}))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(it)).Times(1);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVLinkingTest, LinkRateSigmoidal) {
    const std::string LOG_NAME = "LinkRateSigmoidal";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    config = {{"linking",
               {"recent_screen_cutoff = 3.0", "scaling_coefficient = 3.0",
                "scaling_type = sigmoidal"}}};

    BuildSimConf(test_conf, config);
    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    model_data->AddSource(test_db);

    auto it = data::InfectionType::kHcv;
    hcv.hcv = data::HCV::kAcute;
    linkage.link_state = data::LinkageState::kUnlinked;
    screen.screen_type = data::ScreeningType::kBackground;
    screen.time_of_last_screening = 1;
    screen.identified = true;

    ON_CALL(mock_person, GetLinkageDetails(it)).WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetScreeningDetails(it)).WillByDefault(Return(screen));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(3));

    EXPECT_CALL(mock_sampler, GetDecision({{0.0285977695687718038}}))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(it)).Times(1);

    auto event = event::hcv::Linking::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}
} // namespace testing
} // namespace hepce

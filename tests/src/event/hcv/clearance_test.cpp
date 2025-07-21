////////////////////////////////////////////////////////////////////////////////
// File: clearance_test.cpp                                                   //
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
#include <hepce/event/hcv/clearance.hpp>

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

class HCVClearanceTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
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
    data::TreatmentDetails treatment = {false, -1, 0, 0, 0, 0, 0, false};

    void SetUp() override {
        BuildSimConf(test_conf);
        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    }

    void TearDown() override { std::filesystem::remove(test_conf); }
};

TEST_F(HCVClearanceTest, NotAcute) {
    const std::string LOG_NAME = "NotAcute";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);
    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, GetTreatmentDetails(_)).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Clearance::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVClearanceTest, InTreatment) {
    const std::string LOG_NAME = "InTreatment";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kAcute;
    treatment.initiated_treatment = true;

    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, GetTreatmentDetails(data::InfectionType::kHcv))
        .WillOnce(Return(treatment));
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Clearance::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVClearanceTest, NoClearance) {
    const std::string LOG_NAME = "NoClearance";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kAcute;
    treatment.initiated_treatment = false;

    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, GetTreatmentDetails(data::InfectionType::kHcv))
        .WillOnce(Return(treatment));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));

    EXPECT_CALL(mock_person, ClearHCV(_)).Times(0);

    auto event = event::hcv::Clearance::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVClearanceTest, Clearance) {
    const std::string LOG_NAME = "Clearance";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kAcute;
    treatment.initiated_treatment = false;

    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, GetTreatmentDetails(data::InfectionType::kHcv))
        .WillOnce(Return(treatment));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));

    EXPECT_CALL(mock_person, ClearHCV(true)).Times(1);

    auto event = event::hcv::Clearance::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVClearanceTest, ClearanceProbZero) {
    const std::string LOG_NAME = "ClearanceProbZero";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    config = {{"infection", {"clearance_prob = 0"}}};

    BuildSimConf(test_conf, config);
    model_data = datamanagement::ModelData::Create(test_conf, LOG_NAME);

    hcv.hcv = data::HCV::kAcute;
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_sampler, GetDecision({{0, 1}})).WillOnce(Return(1));

    auto event = event::hcv::Clearance::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);

    std::filesystem::remove(LOG_FILE);
}

} // namespace testing
} // namespace hepce

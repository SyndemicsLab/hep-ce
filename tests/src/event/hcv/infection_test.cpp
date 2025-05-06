////////////////////////////////////////////////////////////////////////////////
// File: infection_test.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/hcv/infection.hpp>

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

class HCVInfectionTest : public ::testing::Test {
protected:
    MockPerson mock_person;
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
    data::ScreeningDetails screen = {-1, 0, 0, false, false, -1};

    void SetUp() override {
        ExecuteQueries(test_db,
                       {{"DROP TABLE IF EXISTS incidence;", CreateIncidence(),
                         "INSERT INTO incidence "
                         "VALUES (25, 0, 4, 0.0102);"}});
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVInfectionTest, AcuteProgression) {
    const std::string LOG_NAME = "AcuteProgression";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kAcute;
    hcv.time_changed = 0;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(3)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(6));
    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Infection::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVInfectionTest, AcuteNeedMoreTime) {
    const std::string LOG_NAME = "AcuteNeedMoreTime";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kAcute;
    hcv.time_changed = 3;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(3)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(6));
    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Infection::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVInfectionTest, AlreadyInfected) {
    const std::string LOG_NAME = "AlreadyInfected";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kChronic;
    hcv.time_changed = 6;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(2)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Infection::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVInfectionTest, NoInfect) {
    const std::string LOG_NAME = "NoInfect";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kNone;
    hcv.time_changed = 0;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(2)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(0);

    EXPECT_CALL(mock_person, GetAge()).WillOnce(Return(300));
    EXPECT_CALL(mock_person, GetSex()).WillOnce(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails()).WillOnce(Return(behaviors));

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);

    auto event = event::hcv::Infection::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVInfectionTest, Infect_NoGeno3) {
    const std::string LOG_NAME = "Infect_NoGeno3";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kNone;
    hcv.time_changed = 0;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(2)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(0);

    EXPECT_CALL(mock_person, GetAge()).WillOnce(Return(300));
    EXPECT_CALL(mock_person, GetSex()).WillOnce(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails()).WillOnce(Return(behaviors));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, InfectHCV()).Times(1);
    EXPECT_CALL(mock_person, SetGenotypeThree(true)).Times(0);

    auto event = event::hcv::Infection::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVInfectionTest, Infect_Geno3) {
    const std::string LOG_NAME = "Infect_Geno3";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    hcv.hcv = data::HCV::kNone;
    hcv.time_changed = 0;

    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(2)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(0);

    EXPECT_CALL(mock_person, GetAge()).WillOnce(Return(300));
    EXPECT_CALL(mock_person, GetSex()).WillOnce(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails()).WillOnce(Return(behaviors));

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, InfectHCV()).Times(1);
    EXPECT_CALL(mock_person, SetGenotypeThree(true)).Times(1);

    auto event = event::hcv::Infection::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

} // namespace testing
} // namespace hepce

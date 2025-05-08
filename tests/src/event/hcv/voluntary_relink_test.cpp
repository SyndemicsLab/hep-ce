////////////////////////////////////////////////////////////////////////////////
// File: voluntary_relink_test.cpp                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/hcv/voluntary_relink.hpp>

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

class HCVVoluntaryRelinkTest : public ::testing::Test {
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
    data::LinkageDetails linkage = {data::LinkageState::kLinked, -1,
                                    data::LinkageType::kIntervention, 0};

    void SetUp() override {
        BuildSimConf(test_conf);
        model_data = datamanagement::ModelData::Create(test_conf);
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVVoluntaryRelinkTest, AlreadyLinked) {
    const std::string LOG_NAME = "AlreadyLinked";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kLinked;
    EXPECT_CALL(mock_person, GetLinkageDetails(_)).WillOnce(Return(linkage));
    EXPECT_CALL(mock_person, Link(_, _)).Times(0);

    auto event = event::hcv::VoluntaryRelink::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVVoluntaryRelinkTest, OutOfTime) {
    const std::string LOG_NAME = "OutOfTime";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    linkage.time_link_change = 0;
    EXPECT_CALL(mock_person, GetLinkageDetails(_))
        .Times(2)
        .WillRepeatedly(Return(linkage));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(6));
    EXPECT_CALL(mock_person, Link(_, _)).Times(0);

    auto event = event::hcv::VoluntaryRelink::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVVoluntaryRelinkTest, NoHCV) {
    const std::string LOG_NAME = "NoHCV";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    linkage.time_link_change = 0;
    hcv.hcv = data::HCV::kNone;

    EXPECT_CALL(mock_person, GetLinkageDetails(_))
        .Times(2)
        .WillRepeatedly(Return(linkage));

    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));

    EXPECT_CALL(mock_person, Link(_, _)).Times(0);

    auto event = event::hcv::VoluntaryRelink::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVVoluntaryRelinkTest, SampleNo) {
    const std::string LOG_NAME = "SampleNo";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    linkage.time_link_change = 0;
    hcv.hcv = data::HCV::kChronic;
    EXPECT_CALL(mock_person, GetLinkageDetails(_))
        .Times(2)
        .WillRepeatedly(Return(linkage));

    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));

    EXPECT_CALL(mock_person, Link(_, _)).Times(0);

    auto event = event::hcv::VoluntaryRelink::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVVoluntaryRelinkTest, Relink) {
    const std::string LOG_NAME = "Relink";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kUnlinked;
    linkage.time_link_change = 0;
    hcv.hcv = data::HCV::kChronic;
    EXPECT_CALL(mock_person, GetLinkageDetails(_))
        .Times(2)
        .WillRepeatedly(Return(linkage));

    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));

    EXPECT_CALL(mock_person, Link(_, _)).Times(1);
    EXPECT_CALL(mock_person, AddRnaScreen(_)).Times(1);
    EXPECT_CALL(mock_person, AddCost(31.22, _, model::CostCategory::kScreening))
        .Times(1);

    auto event = event::hcv::VoluntaryRelink::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}
} // namespace testing
} // namespace hepce

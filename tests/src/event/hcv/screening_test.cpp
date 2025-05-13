////////////////////////////////////////////////////////////////////////////////
// File: screening_test.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/hcv/screening.hpp>

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

class HCVScreeningTest : public ::testing::Test {
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
    data::LinkageDetails linkage = {data::LinkageState::kUnlinked, -1, 0};
    data::ScreeningDetails screen = {
        -1, 0, 0, false, false, -1, data::ScreeningType::kNa};

    void SetUp() override {
        ExecuteQueries(test_db,
                       {{"DROP TABLE IF EXISTS antibody_testing;",
                         "DROP TABLE IF EXISTS screening_and_linkage;",
                         CreateScreeningAndLinkage(), CreateAntibodyTesting(),
                         "INSERT INTO screening_and_linkage "
                         "VALUES (25, 1, 4, -1, 0, 0.02, 1, 0.3);",
                         "INSERT INTO screening_and_linkage "
                         "VALUES (25, 1, 4, 1, 0, 0.03, 1, 0.4);",
                         "INSERT INTO antibody_testing "
                         "VALUES (25, 4, 1.0);"}});
        BuildSimConf(test_conf);
        discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
        discounted_life = utils::Discount(1, 0.0025, 1, false);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex())
            .WillByDefault(Return(data::Sex::kFemale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behaviors));
        ON_CALL(mock_person, IsBoomer()).WillByDefault(Return(false));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetLinkageDetails(_))
            .WillByDefault(Return(linkage));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVScreeningTest, Linked) {
    const std::string LOG_NAME = "Linked";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    linkage.link_state = data::LinkageState::kLinked;
    EXPECT_CALL(mock_person, GetLinkageDetails(_)).WillOnce(Return(linkage));

    EXPECT_CALL(mock_person, GetCurrentTimestep()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::hcv::Screening::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVScreeningTest, OneTime_NotFirstTimestep) {
    const std::string LOG_NAME = "OneTime_NotFirstTimestep";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(1)
        .WillRepeatedly(Return(6));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));

    auto event = event::hcv::Screening::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVScreeningTest, OneTime_FirstTimestep) {
    const std::string LOG_NAME = "OneTime_FirstTimestep";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));

    auto event = event::hcv::Screening::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVScreeningTest, InterventionScreen_NegativeRNA) {
    const std::string LOG_NAME = "InterventionScreen";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    screen.identified = true;

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(1));

    EXPECT_CALL(mock_person, GetScreeningDetails(_))
        .Times(1)
        .WillRepeatedly(Return(screen));
    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(1)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, Screen(_, data::ScreeningTest::kRna,
                                    data::ScreeningType::kIntervention))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(31.22, _, model::CostCategory::kScreening))
        .Times(1);
    EXPECT_CALL(mock_person, Diagnose(_)).Times(0);

    auto event = event::hcv::Screening::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(HCVScreeningTest, InterventionScreen_PositiveRNA) {
    const std::string LOG_NAME = "InterventionScreen";
    const std::string LOG_FILE = LOG_NAME + ".log";
    hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE);

    screen.identified = true;

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(0));

    EXPECT_CALL(mock_person, GetScreeningDetails(_))
        .Times(1)
        .WillRepeatedly(Return(screen));
    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(1)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, Screen(_, data::ScreeningTest::kRna,
                                    data::ScreeningType::kIntervention))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(31.22, _, model::CostCategory::kScreening))
        .Times(1);
    EXPECT_CALL(mock_person, Diagnose(_)).Times(1);

    auto event = event::hcv::Screening::Create(*model_data, LOG_NAME);
    event->Execute(mock_person, mock_sampler);
    std::filesystem::remove(LOG_FILE);
}

} // namespace testing
} // namespace hepce

////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_test.cpp                                                   //
// Project: hep-ce                                                            //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/behavior/pregnancy.hpp>

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

class PregnancyTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    double discounted_cost;
    double discounted_life;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};

    void SetUp() override {
        ExecuteQueries(test_db,
                       {{"DROP TABLE IF EXISTS pregnancy;", CreatePregnancy(),
                         "INSERT INTO pregnancy "
                         "VALUES (25, 0.008, 0.0005);"}});
        BuildSimConf(test_conf);
        discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
        discounted_life = utils::Discount(1, 0.0025, 1, false);
        model_data = datamanagement::ModelData::Create(test_conf);
        model_data->AddSource(test_db);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetSex())
            .WillByDefault(Return(data::Sex::kFemale));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(PregnancyTest, Males) {
    EXPECT_CALL(mock_person, GetSex()).WillOnce(Return(data::Sex::kMale));
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, TooYoung) {
    EXPECT_CALL(mock_person, GetAge()).Times(1).WillRepeatedly(Return(5));
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, TooOld) {
    EXPECT_CALL(mock_person, GetAge()).Times(2).WillRepeatedly(Return(600));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(1)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, TooOldPregnant) {
    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetAge()).Times(3).WillRepeatedly(Return(541));
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(7)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, TooOldPostpartum) {
    pregnancy.pregnancy_state = data::PregnancyState::kRestrictedPostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetAge()).Times(2).WillRepeatedly(Return(541));
    EXPECT_CALL(mock_person, GetCurrentTimestep()).WillOnce(Return(1));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(3)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, TooShortPostpartum) {
    pregnancy.pregnancy_state = data::PregnancyState::kRestrictedPostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep()).WillOnce(Return(1));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(3)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ProgressYearOnePostpartum) {
    pregnancy.pregnancy_state = data::PregnancyState::kRestrictedPostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep()).WillOnce(Return(3));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(6)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_person,
                SetPregnancyState(data::PregnancyState::kYearOnePostpartum))
        .Times(1);
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ProgressYearTwoPostpartum) {
    pregnancy.pregnancy_state = data::PregnancyState::kYearOnePostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(12));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(6)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_person,
                SetPregnancyState(data::PregnancyState::kYearTwoPostpartum))
        .Times(1);
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, EndPostpartum) {
    pregnancy.pregnancy_state = data::PregnancyState::kYearTwoPostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(24));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(6)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_person, EndPostpartum()).Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, Impregnate) {
    pregnancy.pregnancy_state = data::PregnancyState::kNone;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep()).WillOnce(Return(15));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(6)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_person, EndPostpartum()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Impregnate()).Times(1);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, HaveSingleChild) {
    data::HCVDetails hcv = {data::HCV::kNone,
                            data::FibrosisState::kNone,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};

    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(9));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(7)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .Times(2)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, Birth(_)).Times(1);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, Stillbirth) {
    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(9));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(7)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Stillbirth()).Times(1);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, HaveMultipleChildren) {
    data::HCVDetails hcv = {data::HCV::kNone,
                            data::FibrosisState::kNone,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};

    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(9));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(7)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, GetHCVDetails())
        .Times(2)
        .WillRepeatedly(Return(hcv));
    EXPECT_CALL(mock_person, Birth(_)).Times(2);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, HaveChronicHCVChild_NotTested) {
    data::HCVDetails hcv = {data::HCV::kChronic,
                            data::FibrosisState::kNone,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};

    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(9));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(7)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .Times(4)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, AddInfantExposure()).Times(1);
    EXPECT_CALL(mock_person, Birth(_)).Times(1);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, HaveChronicHCVChild_Tested) {
    data::HCVDetails hcv = {data::HCV::kChronic,
                            data::FibrosisState::kNone,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};

    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    EXPECT_CALL(mock_person, GetCurrentTimestep())
        .Times(2)
        .WillRepeatedly(Return(9));
    EXPECT_CALL(mock_person, GetPregnancyDetails())
        .Times(7)
        .WillRepeatedly(Return(pregnancy));
    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, GetHCVDetails()).WillOnce(Return(hcv));
    EXPECT_CALL(mock_person, AddInfantExposure()).Times(1);
    EXPECT_CALL(mock_person, Birth(_)).Times(1);

    auto event = event::behavior::Pregnancy::Create(*model_data);
    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

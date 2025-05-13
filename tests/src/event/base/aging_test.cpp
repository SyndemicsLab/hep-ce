////////////////////////////////////////////////////////////////////////////////
// File: AgingTest.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/base/aging.hpp>

// STL Includes
#include <memory>
#include <string>
#include <vector>

// 3rd Party Dependencies
#include <datamanagement/datamanagement.hpp>
#include <gtest/gtest.h>

// Library Headers
#include <hepce/utils/math.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include <config.hpp>
#include <inputs_db.hpp>
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::Return;

using namespace hepce::data;
using namespace hepce::event;

namespace hepce {
namespace testing {

class AgingTest : public ::testing::Test {
protected:
    MockPerson mock_person;
    MockSampler mock_sampler;
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";
    std::unique_ptr<datamanagement::ModelData> model_data;
    double discounted_cost;
    double discounted_life;
    data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};
    void SetUp() override {
        ExecuteQueries(test_db, {"DROP TABLE IF EXISTS background_impacts;",
                                 CreateBackgroundImpacts(),
                                 "INSERT INTO background_impacts "
                                 "VALUES (25, 0, 4, 0.821, 370.75);"});
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

TEST_F(AgingTest, Execute) {
    // Setup
    EXPECT_CALL(mock_person, GetAge()).WillOnce(Return(300));
    EXPECT_CALL(mock_person, GetSex()).WillOnce(Return(Sex::kMale));
    EXPECT_CALL(mock_person, GetBehaviorDetails()).WillOnce(Return(behaviors));

    // Expectations
    EXPECT_CALL(mock_person, IsAlive()).WillOnce(Return(true));
    EXPECT_CALL(mock_person, Grow()).Times(1);
    EXPECT_CALL(mock_person, GetCurrentTimestep()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person,
                AddCost(370.75, discounted_cost, CostCategory::kBackground))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(0.821, UtilityCategory::kBackground))
        .Times(1);
    EXPECT_CALL(mock_person, AccumulateTotalUtility(0.0025)).Times(1);
    EXPECT_CALL(mock_person, AddDiscountedLifeSpan(discounted_life)).Times(1);

    // Running Test
    auto aging = hepce::event::base::Aging::Create(*model_data);
    aging->Execute(mock_person, mock_sampler);
}

TEST_F(AgingTest, ExecuteDead) {
    // Setup
    EXPECT_CALL(mock_person, GetAge()).Times(0);
    EXPECT_CALL(mock_person, GetSex()).Times(0);
    EXPECT_CALL(mock_person, GetBehaviorDetails()).Times(0);

    // Expectations
    EXPECT_CALL(mock_person, IsAlive()).WillOnce(Return(false));
    EXPECT_CALL(mock_person, Grow()).Times(0);
    EXPECT_CALL(mock_person, GetCurrentTimestep()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);
    EXPECT_CALL(mock_person, AccumulateTotalUtility(_)).Times(0);
    EXPECT_CALL(mock_person, AddDiscountedLifeSpan(_)).Times(0);

    // Running Test
    auto aging = hepce::event::base::Aging::Create(*model_data);
    aging->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

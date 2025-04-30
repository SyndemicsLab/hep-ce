////////////////////////////////////////////////////////////////////////////////
// File: AgingTest.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/base/aging.hpp>

// STL Includes
#include <cstdio>
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
#include "config.hpp"
#include "inputs_db.hpp"
#include "person_mock.hpp"
#include "sampler_mock.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

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
    void SetUp() override {
        data::BehaviorDetails behaviors = {data::Behavior::kInjection, 0};

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(Sex::kMale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behaviors));

        ExecuteQueries(test_db, {CreateBackgroundImpacts(),
                                 "INSERT INTO background_impacts "
                                 "VALUES (25, 0, 4, 0.821, 370.75);"});
        BuildSimConf(test_conf);
    }
    void TearDown() override {
        std::remove(test_db.c_str());
        std::remove(test_conf.c_str());
    }
};

std::string SQLQuery = "SELECT age_years, gender, drug_behavior, cost, utility "
                       "FROM background_impacts;";

TEST_F(AgingTest, Execute) {
    double discounted_cost = utils::Discount(370.75, 0.0025, 1, false);
    double discounted_life = utils::Discount(1, 0.0025, 1, false);
    auto model_data = datamanagement::ModelData(test_conf);

    // Expectations
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
    auto aging = hepce::event::base::Aging::Create(model_data);
    aging->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

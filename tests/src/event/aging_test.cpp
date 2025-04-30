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
#include <memory>
#include <string>
#include <vector>

// 3rd Party Dependencies
#include <gtest/gtest.h>

// Library Headers
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
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

class AgingTest : public ::testing::Test {
protected:
    NiceMock<hepce::model::MockPerson> mock_person;
    NiceMock<hepce::model::MockSampler> mock_sampler;
    void SetUp() override {
        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    }
    void TearDown() override {}
};

std::string SQLQuery = "SELECT age_years, gender, drug_behavior, cost, utility "
                       "FROM background_impacts;";

TEST_F(AgingTest, Aging) {
    // Person Setup
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
    ON_CALL(mock_person, GetSex()).WillByDefault(Return(Sex::kMale));
    ON_CALL(mock_person, GetBehavior())
        .WillByDefault(Return(Behavior::kInjection));

    // Data Setup
    double discount_rate = 0.025;
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(discount_rate)), Return(0)));
    Utils::tuple_3i tup = std::make_tuple(25, 0, 4);
    struct cost_util cu = {25.00, 0.5};
    std::unordered_map<Utils::tuple_3i, struct cost_util, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        custorage;
    custorage[tup] = cu;
    ON_CALL(*event_dm, SelectCustomCallback(SQLQuery, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_CU(&custorage), Return(0)));

    double discounted_cost = Utils::discount(cu.cost, discount_rate, 1);
    std::pair<double, double> expected_utils = {cu.util, cu.util};
    std::pair<double, double> expected_discounted_utils = {
        Utils::discount(0.5, discount_rate, 1),
        Utils::discount(0.5, discount_rate, 1)};
    double expected_discounted_life = Utils::discount(1, discount_rate, 1);

    // Expectations
    EXPECT_CALL(mock_person, Grow()).Times(1);
    EXPECT_CALL(mock_person, GetCurrentTimestep()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, AddCost(cu.cost, discounted_cost,
                                     cost::CostCategory::kBackground))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(cu.util, utility::UtilityCategory::kBackground))
        .Times(1);
    EXPECT_CALL(mock_person, GetUtility()).WillOnce(Return(expected_utils));
    EXPECT_CALL(mock_person, AccumulateTotalUtility(expected_utils,
                                                    expected_discounted_utils))
        .Times(1);
    EXPECT_CALL(mock_person, AddDiscountedLifeSpan(expected_discounted_life))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Aging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

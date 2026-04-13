////////////////////////////////////////////////////////////////////////////////
// File: behavior_changes_test.cpp                                            //
// Project: hep-ce                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <filesystem>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <config.hpp>
#include <inputs_db.hpp>
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class BehaviorChangesTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::BehaviorDetails behavior = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud = {data::MOUD::kNone, -1, 0, 0};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(test_db, {"DROP TABLE IF EXISTS behavior_transitions;",
                                 CreateBehaviorTransitions(),
                                 "INSERT INTO behavior_transitions VALUES "
                                 "(25, 0, 4, 0, 0.0, 0.0, 0.0, 0.0, 1.0);",
                                 "INSERT INTO behavior_transitions VALUES "
                                 "(25, 0, 2, 0, 0.0, 0.0, 0.7, 0.0, 0.3);",
                                 "INSERT INTO behavior_transitions VALUES "
                                 "(25, 0, 1, 0, 0.0, 0.8, 0.0, 0.2, 0.0);"});

        ExecuteQueries(
            test_db,
            {"DROP TABLE IF EXISTS behavior_impacts;", CreateBehaviorImpacts(),
             "INSERT INTO behavior_impacts VALUES (0, 4, 11.0, 0.7);",
             "INSERT INTO behavior_impacts VALUES (0, 2, 8.0, 0.8);",
             "INSERT INTO behavior_impacts VALUES (0, 1, 7.0, 0.85);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(24));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(BehaviorChangesTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, SetBehavior(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(BehaviorChangesTest, InvalidDecisionReturnsBeforeApplyingState) {
    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeBadDecision");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(99));
    EXPECT_CALL(mock_person, SetBehavior(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kBehavior))
        .Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(BehaviorChangesTest,
       MissingTransitionTupleFallsBackToGuaranteedInjection) {
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(312));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeFallback");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(
                                  DoubleEq(0.0), DoubleEq(0.0), DoubleEq(0.0),
                                  DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kBehavior))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(BehaviorChangesTest, FormerInjectionAppliesDecayAndTransitions) {
    behavior.behavior = data::Behavior::kFormerInjection;
    behavior.time_last_active = 0;
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeDecay");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kBehavior))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(BehaviorChangesTest, MissingCostTableFallsBackToZeroCostUtility) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS behavior_impacts;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeNoCost");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);
    EXPECT_CALL(mock_person, AddCost(DoubleEq(0.0), DoubleEq(0.0),
                                     model::CostCategory::kBehavior))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.0), model::UtilityCategory::kBehavior))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(BehaviorChangesTest,
       MissingTransitionsTableFallsBackToGuaranteedInjection) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS behavior_transitions;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeNoTransitions");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(
                                  DoubleEq(0.0), DoubleEq(0.0), DoubleEq(0.0),
                                  DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(BehaviorChangesTest,
       EmptyTransitionsTableFallsBackToGuaranteedInjection) {
    ExecuteQueries(test_db, {"DELETE FROM behavior_transitions;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("BehaviorChanges", inputs,
                                                  "BehChangeEmptyTransitions");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(
                                  DoubleEq(0.0), DoubleEq(0.0), DoubleEq(0.0),
                                  DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(4));
    EXPECT_CALL(mock_person, SetBehavior(data::Behavior::kInjection)).Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

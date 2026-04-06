////////////////////////////////////////////////////////////////////////////////
// File: moud_test.cpp                                                        //
// Project: hep-ce                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <filesystem>
#include <memory>
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

class MoudTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::BehaviorDetails behavior = {data::Behavior::kInjection, 0};
    data::MOUDDetails moud = {data::MOUD::kNone, -1, 0, 0};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(
            test_db,
            {"DROP TABLE IF EXISTS moud_transitions;", CreateMoudTransitions(),
             "INSERT INTO moud_transitions VALUES (25, 0, 0, -1, 1.0, 0.0, "
             "0.0);",
             "INSERT INTO moud_transitions VALUES (25, 1, 4, -1, 0.0, 0.0, "
             "1.0);",
             "INSERT INTO moud_transitions VALUES (25, 2, 0, -1, 1.0, 0.0, "
             "0.0);"});

        ExecuteQueries(
            test_db,
            {"DROP TABLE IF EXISTS moud_costs;",
             "CREATE TABLE moud_costs(moud INTEGER NOT NULL, pregnancy INTEGER "
             "NOT NULL, cost REAL NOT NULL, utility REAL NOT NULL, PRIMARY "
             "KEY(moud, pregnancy));",
             "INSERT INTO moud_costs VALUES (0, -1, 1.0, 0.99);",
             "INSERT INTO moud_costs VALUES (1, -1, 2.0, 0.95);",
             "INSERT INTO moud_costs VALUES (2, -1, 3.0, 0.90);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(MoudTest, ReturnsEarlyWhenNoHistoryOfOud) {
    behavior.behavior = data::Behavior::kNever;
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("MOUD", inputs, "MoudNoHistory");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, PostTreatmentTransitionsImmediately) {
    behavior.behavior = data::Behavior::kInjection;
    moud.moud_state = data::MOUD::kPost;
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("MOUD", inputs, "MoudPost");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kMoud))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, CurrentTreatmentCanTransitionToPostViaSampling) {
    behavior.behavior = data::Behavior::kInjection;
    moud.moud_state = data::MOUD::kCurrent;
    moud.current_state_concurrent_months = 4;
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("MOUD", inputs, "MoudCurrent");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kMoud))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, NoneStateCanTransitionToCurrentViaSampling) {
    behavior.behavior = data::Behavior::kInjection;
    moud.moud_state = data::MOUD::kNone;
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("MOUD", inputs, "MoudNone");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kMoud))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, MissingTransitionTupleFallsBackToGuaranteedPostProbability) {
    behavior.behavior = data::Behavior::kInjection;
    moud.moud_state = data::MOUD::kNone;
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(312));
    ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));
    ON_CALL(mock_person, GetMoudDetails()).WillByDefault(Return(moud));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("MOUD", inputs, "MoudFallback");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(
                                  DoubleEq(0.0), DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(2));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, MissingTransitionTableStillExecutesWithFallbackTransition) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS moud_transitions;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("MOUD", inputs,
                                                  "MoudNoTransitionsTable");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(
                                  DoubleEq(0.0), DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(2));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, EmptyTransitionTableStillExecutesWithFallbackTransition) {
    ExecuteQueries(test_db, {"DELETE FROM moud_transitions;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("MOUD", inputs,
                                                  "MoudEmptyTransitions");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(
                                  DoubleEq(0.0), DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(2));
    EXPECT_CALL(mock_person, TransitionMOUD()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, MissingCostTableFallsBackToDefaultCostAndUtility) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS moud_costs;"});

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("MOUD", inputs, "MoudNoCostTable");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, AddCost(DoubleEq(0.0), DoubleEq(0.0),
                                     model::CostCategory::kMoud))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.0), model::UtilityCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(MoudTest, EmptyCostTableFallsBackToDefaultCostAndUtility) {
    ExecuteQueries(test_db, {"DELETE FROM moud_costs;"});

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("MOUD", inputs, "MoudEmptyCost");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, AddCost(DoubleEq(0.0), DoubleEq(0.0),
                                     model::CostCategory::kMoud))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.0), model::UtilityCategory::kMoud))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

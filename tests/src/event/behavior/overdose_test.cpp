////////////////////////////////////////////////////////////////////////////////
// File: overdose_test.cpp                                                    //
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
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class OverdoseTest : public ::testing::Test {
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

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS overdoses;", CreateOverdoses(),
                        "INSERT INTO overdoses VALUES (-1, 0, 4, 0.5, 10.00, "
                        "0.2);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
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

TEST_F(OverdoseTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Overdose", inputs, "OverdoseDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, ToggleOverdose()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(OverdoseTest, TogglesAndAppliesCostUtilityWhenOverdoseOccurs) {
    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Overdose", inputs, "OverdoseExec");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, ToggleOverdose()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kOverdose))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kOverdose))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(OverdoseTest, DoesNothingWhenNoOverdoseOccurs) {
    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Overdose", inputs, "OverdoseNoExec");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, ToggleOverdose()).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

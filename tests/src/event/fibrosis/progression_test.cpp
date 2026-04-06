////////////////////////////////////////////////////////////////////////////////
// File: progression_test.cpp                                                 //
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

class ProgressionTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::HCVDetails hcv = {data::HCV::kAcute,
                            data::FibrosisState::kF0,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::ScreeningDetails screening = {-1, 0, 0, false, false, -1, 0};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS hcv_impacts;", CreateHCVImpacts(),
                        "INSERT INTO hcv_impacts VALUES (0, 0, 230.65, 0.915);",
                        "INSERT INTO hcv_impacts VALUES (1, 0, 430.00, 0.8);",
                        "INSERT INTO hcv_impacts VALUES (1, 1, 500.00, 0.7);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(screening));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(ProgressionTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, NonInfectedOnlyResolvesCostAndUtility) {
    hcv.hcv = data::HCV::kNone;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgNone");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kLiver))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, ProgressesFibrosisWhenSampled) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgExec");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, SetFibrosis(data::FibrosisState::kF1)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kLiver))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, DoesNotProgressWhenSampledAsNoProgression) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgNoTransition");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kLiver))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, ProgressesFromF4ToDecompWhenSampled) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF4;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgF4ToDecomp");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, SetFibrosis(data::FibrosisState::kDecomp))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, UsesDefaultTransitionProbabilitiesForUnknownFibrosis) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kNone;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgDefaultProb");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler,
                GetDecision(ElementsAre(DoubleEq(0.0), DoubleEq(1.0))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, UsesF1TransitionProbabilityBranch) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF1;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgF1Prob");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler,
                GetDecision(ElementsAre(DoubleEq(0.00681), DoubleEq(0.99319))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, UsesF2TransitionProbabilityBranch) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF2;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgF2Prob");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.0097026),
                                                      DoubleEq(0.9902974))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, UsesF3TransitionProbabilityBranch) {
    hcv.hcv = data::HCV::kAcute;
    hcv.fibrosis_state = data::FibrosisState::kF3;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgF3Prob");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.0096201),
                                                      DoubleEq(0.9903799))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, HandlesMissingImpactTableAndUsesDefaultCostUtility) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS hcv_impacts;"});
    hcv.hcv = data::HCV::kNone;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgNoImpactTable");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kLiver))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(ProgressionTest, HandlesEmptyImpactTableAndUsesDefaultCostUtility) {
    ExecuteQueries(test_db, {"DELETE FROM hcv_impacts;"});
    hcv.hcv = data::HCV::kNone;
    hcv.fibrosis_state = data::FibrosisState::kF0;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisProgression", inputs,
                                                  "ProgEmptyImpactTable");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLiver))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kLiver))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

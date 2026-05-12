////////////////////////////////////////////////////////////////////////////////
// File: staging_test.cpp                                                     //
// Project: hep-ce                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <config.hpp>
#include <inputs_db.hpp>
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class StagingTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::HCVDetails hcv = {data::HCV::kChronic,
                            data::FibrosisState::kF1,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::StagingDetails staging = {data::MeasuredFibrosisState::kNone, false,
                                    -1};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(
            test_db, {"DROP TABLE IF EXISTS fibrosis;", CreateFibrosis(),
                      "INSERT INTO fibrosis VALUES (0, 0, 0.9, 0.8, 0.7, 0.6, "
                      "1.0);",
                      "INSERT INTO fibrosis VALUES (0, 1, 0.05, 0.1, 0.1, "
                      "0.1, 0.0);",
                      "INSERT INTO fibrosis VALUES (0, 2, 0.03, 0.06, 0.1, "
                      "0.1, 0.0);",
                      "INSERT INTO fibrosis VALUES (0, 3, 0.02, 0.04, 0.1, "
                      "0.1, 0.0);",
                      "INSERT INTO fibrosis VALUES (1, 0, 0.8, 0.7, 0.6, 0.5, "
                      "1.0);",
                      "INSERT INTO fibrosis VALUES (1, 1, 0.1, 0.15, 0.15, "
                      "0.2, 0.0);",
                      "INSERT INTO fibrosis VALUES (1, 2, 0.06, 0.1, 0.15, "
                      "0.2, 0.0);",
                      "INSERT INTO fibrosis VALUES (1, 3, 0.04, 0.05, 0.1, "
                      "0.1, 0.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(12));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
        ON_CALL(mock_person, GetFibrosisStagingDetails())
            .WillByDefault(Return(staging));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(StagingTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisStaging", inputs,
                                                  "StageDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, DiagnoseFibrosis(_)).Times(0);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(StagingTest, ReturnsEarlyWhenNoFibrosisState) {
    hcv.fibrosis_state = data::FibrosisState::kNone;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisStaging", inputs,
                                                  "StageNoFib");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, DiagnoseFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(StagingTest, ReturnsEarlyWhenStagedTooRecently) {
    staging.time_of_last_staging = 11;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(12));
    ON_CALL(mock_person, GetFibrosisStagingDetails())
        .WillByDefault(Return(staging));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisStaging", inputs,
                                                  "StageRecent");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, DiagnoseFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(StagingTest, LatestMethodUsesSecondTestResultWhenEligible) {
    hcv.fibrosis_state = data::FibrosisState::kF1;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisStaging", inputs,
                                                  "StageLatest");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(2));
    EXPECT_CALL(mock_person, DiagnoseFibrosis(_)).Times(2);
    EXPECT_CALL(mock_person, GiveSecondStagingTest()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kStaging))
        .Times(2);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(StagingTest, InvalidFibrosisStateReturnsOnEmptyProbabilityBuilder) {
    hcv.fibrosis_state = static_cast<data::FibrosisState>(999);
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisStaging", inputs,
                                                  "StageInvalidFib");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, DiagnoseFibrosis(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(StagingTest, InvalidMultiTestMethodReturnsAfterSecondTestDecision) {
    std::unordered_map<std::string, std::vector<std::string>> config =
        DEFAULT_CONFIG;
    config["fibrosis_staging"] = {"period = 12",
                                  "test_one = fib4",
                                  "test_one_cost = 0",
                                  "test_two = fibroscan",
                                  "test_two_cost = 140",
                                  "multitest_result_method = bogus",
                                  "test_two_eligible_stages = f1,f2,f3"};
    BuildSimConf(test_conf, config);

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("FibrosisStaging", inputs,
                                                  "StageBadMethod");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, DiagnoseFibrosis(_)).Times(1);
    EXPECT_CALL(mock_person, GiveSecondStagingTest()).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kStaging))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

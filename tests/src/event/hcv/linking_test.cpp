////////////////////////////////////////////////////////////////////////////////
// File: linking_test.cpp                                                     //
// Project: hep-ce                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <config.hpp>
#include <inputs_db.hpp>
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HCVLinkingTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kUnlinked, 0, 0};
    data::ScreeningDetails screening = {-1, 0, 0, false, true, -1, 0,
                                        data::ScreeningType::kBackground, 0,
                                        0};
    data::HCVDetails hcv = {data::HCV::kChronic,
                            data::FibrosisState::kF0,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::BehaviorDetails behavior = {data::Behavior::kInjection, -1};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS screening_and_linkage;",
                        CreateScreeningAndLinkage(),
                        "INSERT INTO screening_and_linkage VALUES "
                        "(25,0,4,-1,0.5,0.6,0.5,1.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetBehaviorDetails()).WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetPregnancyDetails()).WillByDefault(Return(pregnancy));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
        ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(linkage));
        ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(screening));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVLinkingTest, ReturnsEarlyWhenAlreadyLinked) {
    linkage.link_state = data::LinkageState::kLinked;
    ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(linkage));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVLinked");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVLinkingTest, ReturnsEarlyWhenNotIdentified) {
    screening.identified = false;
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVNoId");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVLinkingTest, FalsePositiveWhenUninfectedAddsFalsePositiveCost) {
    hcv.hcv = data::HCV::kNone;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVFalsePos");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, FalsePositive(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLinking))
        .Times(1);
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVLinkingTest, InterventionLinkSuccessAddsInterventionCost) {
    screening.screen_type = data::ScreeningType::kIntervention;
    screening.time_of_last_screening = 1;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVInterv");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kLinking))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVLinkingTest, MultiplierScalingPathExecutesForRecentScreen) {
    screening.screen_type = data::ScreeningType::kBackground;
    screening.time_of_last_screening = 1;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVScaled");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVLinkingTest, ExponentialScalingPathExecutes) {
    std::unordered_map<std::string, std::vector<std::string>> config =
        DEFAULT_CONFIG;
    config["linking"] = {"intervention_cost = 0",
                         "false_positive_test_cost = 442.39",
                         "scaling_type = exponential",
                         "scaling_coefficient = 1.1",
                         "recent_screen_cutoff = 0"};
    BuildSimConf(test_conf, config);

    screening.screen_type = data::ScreeningType::kBackground;
    screening.time_of_last_screening = 0;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVExp");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVLinkingTest, SigmoidalScalingPathExecutes) {
    std::unordered_map<std::string, std::vector<std::string>> config =
        DEFAULT_CONFIG;
    config["linking"] = {"intervention_cost = 0",
                         "false_positive_test_cost = 442.39",
                         "scaling_type = sigmoidal",
                         "scaling_coefficient = 1.1",
                         "recent_screen_cutoff = -1"};
    BuildSimConf(test_conf, config);

    screening.screen_type = data::ScreeningType::kBackground;
    screening.time_of_last_screening = 0;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVLinking", inputs, "HCVSig");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

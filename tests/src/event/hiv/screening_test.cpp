////////////////////////////////////////////////////////////////////////////////
// File: screening_test.cpp                                                   //
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
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HIVScreeningTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kUnlinked, -1, 0};
    data::ScreeningDetails screening = {
        -1, 0, 0, false, false, -1, 0, data::ScreeningType::kNa, 0, 0};
    data::HCVDetails hcv = {data::HCV::kNone,
                            data::FibrosisState::kF0,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::BehaviorDetails behavior = {data::Behavior::kInjection, -1};

    void SetUp() override {
        std::unordered_map<std::string, std::vector<std::string>> config =
            DEFAULT_CONFIG;
        config["hiv_screening"] = {"intervention_type = periodic",
                                   "period = 12"};
        BuildSimConf(test_conf, config);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS screening_and_linkage;",
                        CreateScreeningAndLinkage(),
                        "INSERT INTO screening_and_linkage VALUES "
                        "(25,0,4,-1,1.0,1.0,1.0,1.0);",
                        "DROP TABLE IF EXISTS antibody_testing;",
                        CreateAntibodyTesting(),
                        "INSERT INTO antibody_testing VALUES (25,4,1.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, IsBoomer()).WillByDefault(Return(false));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
        ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHiv))
            .WillByDefault(Return(linkage));
        ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHiv))
            .WillByDefault(Return(screening));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HIVScreeningTest, PeriodicInterventionPathRunsAtFirstTimestep) {
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVScreening", inputs,
                                                  "HIVScrPeriodic");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHiv, data::ScreeningTest::kAb,
                       data::ScreeningType::kIntervention))
        .Times(1);
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHiv, data::ScreeningTest::kRna,
                       data::ScreeningType::kIntervention))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HIVScreeningTest,
       PeriodicBackgroundPathWhenNotFirstAndPeriodNotReached) {
    screening.time_of_last_screening = 10;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(12));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHiv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVScreening", inputs,
                                                  "HIVScrBackground");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHiv, data::ScreeningTest::kAb,
                       data::ScreeningType::kBackground))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

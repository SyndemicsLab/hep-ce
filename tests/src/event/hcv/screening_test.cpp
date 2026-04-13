////////////////////////////////////////////////////////////////////////////////
// File: screening_test.cpp                                                   //
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
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HCVScreeningTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kUnlinked, -1, 0};
    data::ScreeningDetails screening = {
        -1, 0, 0, false, false, -1, 0, data::ScreeningType::kNa, 0, 0};
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

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS screening_and_linkage;",
                        CreateScreeningAndLinkage(),
                        "INSERT INTO screening_and_linkage VALUES "
                        "(25,0,4,-1,1.0,1.0,1.0,1.0);",
                        "DROP TABLE IF EXISTS antibody_testing;",
                        CreateAntibodyTesting(),
                        "INSERT INTO antibody_testing VALUES (25,4,1.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, IsBoomer()).WillByDefault(Return(false));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(linkage));
        ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(screening));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVScreeningTest, ReturnsEarlyWhenAlreadyLinked) {
    linkage.link_state = data::LinkageState::kLinked;
    ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(linkage));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVScreening", inputs,
                                                  "HCVScrLinked");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Screen(_, _, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVScreeningTest, OneTimeInterventionDiagnosesOnPositiveAbAndRna) {
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVScreening", inputs,
                                                  "HCVScrInterv");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHcv, data::ScreeningTest::kAb,
                       data::ScreeningType::kIntervention))
        .Times(1);
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHcv, data::ScreeningTest::kRna,
                       data::ScreeningType::kIntervention))
        .Times(1);
    EXPECT_CALL(mock_person, Diagnose(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kScreening))
        .Times(2);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVScreeningTest, InterventionSkipsWhenAlreadyIdentified) {
    screening.identified = true;
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVScreening", inputs, "HCVScrSkip");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Screen(_, _, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVScreeningTest, BackgroundFalseNegativeClearsDiagnosisWhenInfected) {
    screening.identified = true;
    screening.ab_positive = false;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVScreening", inputs, "HCVScrFN");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddFalseNegative(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person,
                AddIdentificationsCleared(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, ClearDiagnosis(data::InfectionType::kHcv))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVScreeningTest, BackgroundRnaPathWhenAntibodyAlreadyPositive) {
    screening.ab_positive = true;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVScreening", inputs,
                                                  "HCVScrRnaOnly");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHcv, data::ScreeningTest::kAb,
                       data::ScreeningType::kBackground))
        .Times(0);
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHcv, data::ScreeningTest::kRna,
                       data::ScreeningType::kBackground))
        .Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVScreeningTest, MissingScreeningTablesFallsBackToZeroProbability) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS screening_and_linkage;",
                             "DROP TABLE IF EXISTS antibody_testing;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVScreening", inputs,
                                                  "HCVScrNoTables");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Screen(_, _, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

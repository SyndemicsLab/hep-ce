////////////////////////////////////////////////////////////////////////////////
// File: linking_test.cpp                                                     //
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
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HIVLinkingTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kUnlinked, 0, 0};
    data::ScreeningDetails screening = {
        -1, 0, 0, false, true, -1, 0, data::ScreeningType::kBackground, 0, 0};
    data::HIVDetails hiv = {data::HIV::kLoUn, 0, 0};
    data::BehaviorDetails behavior = {data::Behavior::kInjection, -1};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(test_db, {"DROP TABLE IF EXISTS screening_and_linkage;",
                                 CreateScreeningAndLinkage(),
                                 "INSERT INTO screening_and_linkage VALUES "
                                 "(25,0,4,-1,0.5,0.6,0.5,1.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
        ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Return(hiv));
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

TEST_F(HIVLinkingTest, FalsePositiveWhenHivNoneClearsDiagnosisAndCosts) {
    hiv.hiv = data::HIV::kNone;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Return(hiv));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HIVLinking", inputs, "HIVFalsePos");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, ClearDiagnosis(data::InfectionType::kHiv))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kHiv)).Times(1);
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HIVLinkingTest, LinksWhenSampledAndInterventionAddsCost) {
    screening.screen_type = data::ScreeningType::kIntervention;
    ON_CALL(mock_person, GetScreeningDetails(data::InfectionType::kHiv))
        .WillByDefault(Return(screening));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HIVLinking", inputs, "HIVLink");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Link(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kHiv)).Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

////////////////////////////////////////////////////////////////////////////////
// File: pregnancy_test.cpp                                                   //
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

class PregnancyTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNone, -1, 0, 0, 0, 0, 0, 0, {}};
    data::HCVDetails hcv = {data::HCV::kNone,
                            data::FibrosisState::kF0,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS pregnancy;", CreatePregnancy(),
                        "INSERT INTO pregnancy VALUES (25, 0.008, 0.2);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetSex())
            .WillByDefault(Return(data::Sex::kFemale));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(10));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(PregnancyTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);
    EXPECT_CALL(mock_person, Birth(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ReturnsEarlyForMale) {
    ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregMale");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ImpregnatesWhenSampled) {
    pregnancy.pregnancy_state = data::PregnancyState::kNone;
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregImp");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, Impregnate()).Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, PregnantPathTriggersStillbirthWhenSampled) {
    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(10));
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregStill");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Stillbirth()).Times(1);
    EXPECT_CALL(mock_person, Birth(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ProgressesPostpartumToYearTwoWhenThresholdMet) {
    pregnancy.pregnancy_state = data::PregnancyState::kYearOnePostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(12));
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregPost");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person,
                SetPregnancyState(data::PregnancyState::kYearTwoPostpartum))
        .Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ReturnsEarlyWhenUnderMinimumAge) {
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(179));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregTooYoung");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ReturnsEarlyWhenOldAgeAndNotPregnantOrPostpartum) {
    pregnancy.pregnancy_state = data::PregnancyState::kNone;
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));
    ON_CALL(mock_person, GetAge()).WillByDefault(Return(541));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregTooOldNone");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, RestrictedPostpartumProgressesToYearOneAfterWindow) {
    pregnancy.pregnancy_state = data::PregnancyState::kRestrictedPostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(3));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregToYear1");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person,
                SetPregnancyState(data::PregnancyState::kYearOnePostpartum))
        .Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, YearTwoPostpartumEndsAtTwoYears) {
    pregnancy.pregnancy_state = data::PregnancyState::kYearTwoPostpartum;
    pregnancy.time_of_pregnancy_change = 0;
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(24));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregEndPost");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, EndPostpartum()).Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, PregnantBeforeNineMonthsReturnsWithoutDeliveryAttempt) {
    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 5;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(10));
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregTooEarly");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Stillbirth()).Times(0);
    EXPECT_CALL(mock_person, Birth(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, ChronicMotherTwinBirthAddsExposuresAndBirths) {
    pregnancy.pregnancy_state = data::PregnancyState::kPregnant;
    pregnancy.time_of_pregnancy_change = 0;
    hcv.hcv = data::HCV::kChronic;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(10));
    ON_CALL(mock_person, GetPregnancyDetails())
        .WillByDefault(Return(pregnancy));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregTwinBirth");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddInfantExposure()).Times(2);
    EXPECT_CALL(mock_person, Birth(_)).Times(2);
    EXPECT_CALL(mock_person, Stillbirth()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, MissingPregnancyTableFallsBackToZeroProbability) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS pregnancy;"});

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregNoTable");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler,
                GetDecision(ElementsAre(DoubleEq(1.0), DoubleEq(0.0))))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(PregnancyTest, EmptyPregnancyTableFallsBackToZeroProbability) {
    ExecuteQueries(test_db, {"DELETE FROM pregnancy;"});

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("Pregnancy", inputs, "PregEmpty");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler,
                GetDecision(ElementsAre(DoubleEq(1.0), DoubleEq(0.0))))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, Impregnate()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

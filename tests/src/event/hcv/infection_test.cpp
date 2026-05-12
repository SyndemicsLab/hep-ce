////////////////////////////////////////////////////////////////////////////////
// File: infection_test.cpp                                                   //
// Project: hep-ce                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <filesystem>
#include <string>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <config.hpp>
#include <inputs_db.hpp>
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HCVInfectionTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

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
        BuildSimConf(test_conf);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS incidence;", CreateIncidence(),
                        "INSERT INTO incidence VALUES (2, 1, 4, 0.42);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(6));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(24));
        ON_CALL(mock_person, GetSex())
            .WillByDefault(Return(data::Sex::kFemale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Invoke([this]() {
            return hcv;
        }));
        ON_CALL(mock_person, SetHCV(_))
            .WillByDefault(Invoke([this](data::HCV next) { hcv.hcv = next; }));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVInfectionTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVInfection", inputs, "HCVInfDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);
    EXPECT_CALL(mock_person, SetHCV(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVInfectionTest, ConvertsAcuteToChronicAfterSixMonths) {
    hcv.hcv = data::HCV::kAcute;
    hcv.time_changed = 0;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVInfection", inputs,
                                                  "HCVInfAcuteToChronic");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, SetHCV(data::HCV::kChronic)).Times(1);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVInfectionTest, ReturnsEarlyWhenAlreadyChronic) {
    hcv.hcv = data::HCV::kChronic;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVInfection", inputs,
                                                  "HCVInfAlreadyInfected");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVInfectionTest, InfectsAndSetsGenotypeThreeWhenSampled) {
    hcv.hcv = data::HCV::kNone;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVInfection", inputs,
                                                  "HCVInfExecute");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.42))))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.153))))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, InfectHCV()).Times(1);
    EXPECT_CALL(mock_person, SetGenotypeThree(true)).Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVInfectionTest, DoesNotInfectWhenDecisionIsNoInfection) {
    hcv.hcv = data::HCV::kNone;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVInfection", inputs,
                                                  "HCVInfNoInfection");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.42))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.153))))
        .Times(0);
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);
    EXPECT_CALL(mock_person, SetGenotypeThree(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVInfectionTest, MissingIncidenceTableFallsBackToZeroProbability) {
    ExecuteQueries(test_db, {"DROP TABLE IF EXISTS incidence;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVInfection", inputs,
                                                  "HCVInfMissingIncidence");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.0))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.153))))
        .Times(0);
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVInfectionTest, EmptyIncidenceTableFallsBackToZeroProbability) {
    ExecuteQueries(test_db, {"DELETE FROM incidence;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVInfection", inputs,
                                                  "HCVInfEmptyIncidence");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.0))))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_sampler, GetDecision(ElementsAre(DoubleEq(0.153))))
        .Times(0);
    EXPECT_CALL(mock_person, InfectHCV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

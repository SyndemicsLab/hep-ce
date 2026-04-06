////////////////////////////////////////////////////////////////////////////////
// File: infection_test.cpp                                                   //
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

class HIVInfectionTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::HIVDetails hiv = {data::HIV::kNone, -1, 0};
    data::BehaviorDetails behavior = {data::Behavior::kInjection, 0};

    void SetUp() override {
        BuildSimConf(test_conf);

        ExecuteQueries(
            test_db,
            {"DROP TABLE IF EXISTS hiv_incidence;",
             "CREATE TABLE hiv_incidence(age_years INTEGER NOT NULL, "
             "gender INTEGER NOT NULL, drug_behavior INTEGER NOT NULL, "
             "incidence REAL NOT NULL, PRIMARY KEY(age_years, gender, "
             "drug_behavior));",
             "INSERT INTO hiv_incidence VALUES (25, 0, 4, 0.0050);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Return(hiv));
        ON_CALL(mock_person, GetAge()).WillByDefault(Return(300));
        ON_CALL(mock_person, GetSex()).WillByDefault(Return(data::Sex::kMale));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HIVInfectionTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVInfection", inputs,
                                                  "HIVInfectionDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, GetHIVDetails()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, InfectHIV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HIVInfectionTest, ReturnsEarlyWhenAlreadyInfected) {
    hiv.hiv = data::HIV::kHiUn;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Return(hiv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVInfection", inputs,
                                                  "HIVInfectionAlready");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, InfectHIV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HIVInfectionTest, InfectsWhenSampled) {
    hiv.hiv = data::HIV::kNone;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Return(hiv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVInfection", inputs,
                                                  "HIVInfectionExec");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, InfectHIV()).Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HIVInfectionTest, DoesNotInfectWhenNotSampled) {
    hiv.hiv = data::HIV::kNone;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Return(hiv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVInfection", inputs,
                                                  "HIVInfectionNoExec");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, InfectHIV()).Times(0);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

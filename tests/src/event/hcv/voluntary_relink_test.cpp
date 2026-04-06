////////////////////////////////////////////////////////////////////////////////
// File: voluntary_relink_test.cpp                                            //
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

class HCVVoluntaryRelinkTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;
    MockSampler mock_sampler;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kUnlinked, 0, 1};
    data::HCVDetails hcv = {data::HCV::kChronic,
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

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(linkage));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVVoluntaryRelinkTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("VoluntaryRelinking", inputs,
                                                  "VolRelDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Link(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVVoluntaryRelinkTest, RelinksWhenEligibleAndSampled) {
    linkage.link_state = data::LinkageState::kUnlinked;
    linkage.time_link_change = 0;
    hcv.hcv = data::HCV::kChronic;
    ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(linkage));
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("VoluntaryRelinking", inputs,
                                                  "VolRelExec");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person,
                Screen(data::InfectionType::kHcv, data::ScreeningTest::kRna,
                       data::ScreeningType::kBackground))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kScreening))
        .Times(1);
    EXPECT_CALL(mock_person, Link(data::InfectionType::kHcv)).Times(1);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVVoluntaryRelinkTest, DoesNotRelinkWhenOutsideRelinkWindow) {
    linkage.link_state = data::LinkageState::kUnlinked;
    linkage.time_link_change = 0;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(100));
    ON_CALL(mock_person, GetLinkageDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(linkage));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("VoluntaryRelinking", inputs,
                                                  "VolRelWindow");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, Link(_)).Times(0);
    EXPECT_CALL(mock_person, Screen(_, _, _)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

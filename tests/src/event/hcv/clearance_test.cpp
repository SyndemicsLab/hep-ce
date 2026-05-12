////////////////////////////////////////////////////////////////////////////////
// File: clearance_test.cpp                                                   //
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

class HCVClearanceTest : public ::testing::Test {
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
    data::TreatmentDetails treatment = {false, -1, 0, 0, 0, 0, 0, false};

    void SetUp() override {
        BuildSimConf(test_conf);

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
        ON_CALL(mock_person, GetTreatmentDetails(data::InfectionType::kHcv))
            .WillByDefault(Return(treatment));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVClearanceTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("Clearance", inputs,
                                                  "HCVClearanceDead");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, GetHCVDetails()).Times(0);
    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, ClearHCV(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVClearanceTest, ReturnsEarlyWhenNotAcute) {
    hcv.hcv = data::HCV::kNone;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("Clearance", inputs,
                                                  "HCVClearanceNotAcute");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, ClearHCV(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVClearanceTest, ReturnsEarlyWhenInTreatment) {
    hcv.hcv = data::HCV::kAcute;
    treatment.initiated_treatment = true;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetTreatmentDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(treatment));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("Clearance", inputs,
                                                  "HCVClearanceInTreatment");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).Times(0);
    EXPECT_CALL(mock_person, ClearHCV(_)).Times(0);

    event->Execute(mock_person, mock_sampler);
}

TEST_F(HCVClearanceTest, ClearsAcuteInfectionWhenSampled) {
    hcv.hcv = data::HCV::kAcute;
    treatment.initiated_treatment = false;
    ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Return(hcv));
    ON_CALL(mock_person, GetTreatmentDetails(data::InfectionType::kHcv))
        .WillByDefault(Return(treatment));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("Clearance", inputs,
                                                  "HCVClearanceExecute");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, ClearHCV(true)).Times(1);

    event->Execute(mock_person, mock_sampler);
}

} // namespace testing
} // namespace hepce

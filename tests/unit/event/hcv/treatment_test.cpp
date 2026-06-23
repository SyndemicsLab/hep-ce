////////////////////////////////////////////////////////////////////////////////
// File: treatment_test.cpp                                                   //
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
using ::testing::DoubleEq;
using ::testing::ElementsAre;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HCVTreatmentTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kLinked, 0, 1};
    data::TreatmentDetails treatment = {true, 0, 1, 0, 0, 0, 0, false};
    data::HCVDetails hcv = {data::HCV::kChronic,
                            data::FibrosisState::kF0,
                            false,
                            false,
                            -1,
                            -1,
                            0,
                            0,
                            0};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};
    data::BehaviorDetails behavior = {data::Behavior::kInjection, -1};

    void SetUp() override {
        std::unordered_map<std::string, std::vector<std::string>> config =
            DEFAULT_CONFIG;
        config["eligibility"] = {
            "ineligible_drug_use =", "ineligible_fibrosis_stages =",
            "ineligible_pregnancy_states =",
            "ineligible_time_former_threshold =",
            "ineligible_time_since_linked ="};

        BuildSimConf(test_conf, config);

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS treatments;", CreateTreatments(),
                        "INSERT INTO treatments VALUES "
                        "(0, 0, 0, 'baseline', 2, 500.0, 1.0, 0.0, 0.0, 0.0);",
                        "INSERT INTO treatments VALUES "
                        "(1, 0, 0, 'salvage', 2, 700.0, 0.0, 0.0, 0.0, 0.0);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS treatment_initiations;",
                        CreateTreatmentInitializations(),
                        "INSERT INTO treatment_initiations VALUES (-1, 1.0);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS lost_to_follow_up;",
                        CreateLostToFollowUps(),
                        "INSERT INTO lost_to_follow_up VALUES (-1, 0.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
        ON_CALL(mock_person, GetLinkageDetails(_))
            .WillByDefault(
                Invoke([this](data::InfectionType) { return linkage; }));
        ON_CALL(mock_person, GetTreatmentDetails(_))
            .WillByDefault(
                Invoke([this](data::InfectionType) { return treatment; }));
        ON_CALL(mock_person, GetHCVDetails()).WillByDefault(Invoke([this]() {
            return hcv;
        }));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy));
        ON_CALL(mock_person, GetBehaviorDetails())
            .WillByDefault(Return(behavior));
        ON_CALL(mock_person, IsCirrhotic()).WillByDefault(Return(false));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HCVTreatmentTest, ReturnsEarlyWhenPersonIsDead) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVTreatment", inputs, "HCVTrtDead");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, ReturnsEarlyWhenNotLinked) {
    linkage.link_state = data::LinkageState::kUnlinked;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtUnlinked");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, LostToFollowUpStopsBeforeInitiation) {
    treatment.initiated_treatment = false;
    ExecuteQueries(test_db,
                   {"DELETE FROM lost_to_follow_up;",
                    "INSERT INTO lost_to_follow_up VALUES (-1, 1.0);"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtLostToFollowUp");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, Unlink(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(1.0, model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, ToxicityAndWithdrawalPathEndsEngagement) {
    ExecuteQueries(
        test_db,
        {"UPDATE treatments "
         "SET toxicity_prob_if_withdrawal = 1.0, withdrawal = 1.0 "
         "WHERE salvage = 0 AND genotype_three = 0 AND cirrhotic = 0;"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtToxWithdraw");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(0))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, AddToxicReaction(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, AddWithdrawal(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, Unlink(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(AtLeast(3));

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, CompletesTreatmentAndAchievesSVR) {
    treatment.initiated_treatment = true;
    treatment.time_of_treatment_initiation = 0;

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HCVTreatment", inputs, "HCVTrtSVR");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, AddCompletedTreatment(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, AddSVR()).Times(1);
    EXPECT_CALL(mock_person, ClearHCV(false)).Times(1);
    EXPECT_CALL(mock_person, ClearDiagnosis(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, Unlink(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, InitiateTreatment(data::InfectionType::kHcv))
        .Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, FailedPrimaryTreatmentStartsSalvage) {
    treatment.initiated_treatment = true;
    treatment.in_salvage_treatment = false;
    treatment.time_of_treatment_initiation = 0;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtStartSalvage");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCompletedTreatment(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, InitiateTreatment(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHcv)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, FailedSalvageTreatmentQuitsEngagement) {
    treatment.initiated_treatment = true;
    treatment.in_salvage_treatment = true;
    treatment.time_of_treatment_initiation = 0;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtFailSalvage");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCompletedTreatment(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHcv)).Times(1);
    EXPECT_CALL(mock_person, Unlink(data::InfectionType::kHcv)).Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, DoesNotStartTreatmentWhenInitiationDecisionFails) {
    treatment.initiated_treatment = false;

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtInitiationFails");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, InitiateTreatment(data::InfectionType::kHcv))
        .Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kTreatment))
        .Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, StartsTreatmentWhenEligibleAndSampled) {
    treatment.initiated_treatment = false;
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtStarts");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, InitiateTreatment(data::InfectionType::kHcv))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(2);
    EXPECT_CALL(mock_person,
                SetUtility(0.99, model::UtilityCategory::kTreatment))
        .Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HCVTreatmentTest, MissingInitKeyFallsBackToZeroAndReturnsSafely) {
    treatment.initiated_treatment = false;

    // Keep LTFU lookup valid (-1 exists), but force init lookup miss (-1 absent).
    ExecuteQueries(test_db,
                   {"DELETE FROM treatment_initiations;",
                    "INSERT INTO treatment_initiations VALUES (0, 1.0);"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVTrtMissingInitHarness");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(ElementsAre(DoubleEq(0.0))))
        .Times(2)
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, InitiateTreatment(data::InfectionType::kHcv))
        .Times(0);

    event->Execute(mock_person, sampler);
}

TEST(HCVTreatmentLoadDataTest, HandlesMissingTreatmentsTable) {
    const std::string local_conf = "sim.conf";
    const std::string local_db = "inputs.db";
    BuildSimConf(local_conf);

    ExecuteQueries(local_db,
                   {"DROP TABLE IF EXISTS treatments;",
                    "DROP TABLE IF EXISTS treatment_initiations;",
                    "DROP TABLE IF EXISTS lost_to_follow_up;",
                    CreateTreatmentInitializations(),
                    "INSERT INTO treatment_initiations VALUES (-1, 1.0);",
                    CreateLostToFollowUps(),
                    "INSERT INTO lost_to_follow_up VALUES (-1, 0.0);"});

    data::Inputs inputs(local_conf, local_db);
    auto event =
        event::EventFactory::CreateEvent("HCVTreatment", inputs, "HCVNoTreat");
    ASSERT_NE(event, nullptr);

    std::filesystem::remove(local_db);
    std::filesystem::remove(local_conf);
}

TEST(HCVTreatmentLoadDataTest, HandlesMissingTreatmentInitializationTable) {
    const std::string local_conf = "sim.conf";
    const std::string local_db = "inputs.db";
    BuildSimConf(local_conf);

    ExecuteQueries(local_db,
                   {"DROP TABLE IF EXISTS treatments;",
                    "DROP TABLE IF EXISTS treatment_initiations;",
                    "DROP TABLE IF EXISTS lost_to_follow_up;",
                    CreateTreatments(),
                    "INSERT INTO treatments VALUES "
                    "(0, 0, 0, 'baseline', 2, 500.0, 1.0, 0.0, 0.0, 0.0);",
                    CreateLostToFollowUps(),
                    "INSERT INTO lost_to_follow_up VALUES (-1, 0.0);"});

    data::Inputs inputs(local_conf, local_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVNoTreatInit");
    ASSERT_NE(event, nullptr);

    std::filesystem::remove(local_db);
    std::filesystem::remove(local_conf);
}

TEST(HCVTreatmentLoadDataTest, HandlesEmptyTreatmentTables) {
    const std::string local_conf = "sim.conf";
    const std::string local_db = "inputs.db";
    BuildSimConf(local_conf);

    ExecuteQueries(local_db,
                   {"DROP TABLE IF EXISTS treatments;",
                    "DROP TABLE IF EXISTS treatment_initiations;",
                    "DROP TABLE IF EXISTS lost_to_follow_up;",
                    CreateTreatments(), CreateTreatmentInitializations(),
                    CreateLostToFollowUps(),
                    "INSERT INTO lost_to_follow_up VALUES (-1, 0.0);"});

    data::Inputs inputs(local_conf, local_db);
    auto event = event::EventFactory::CreateEvent("HCVTreatment", inputs,
                                                  "HCVEmptyTreatTables");
    ASSERT_NE(event, nullptr);

    std::filesystem::remove(local_db);
    std::filesystem::remove(local_conf);
}

} // namespace testing
} // namespace hepce

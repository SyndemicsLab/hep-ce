////////////////////////////////////////////////////////////////////////////////
// File: treatment_test.cpp                                                   //
// Project: hep-ce                                                            //
// Created Date: 2026-04-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include <memory>
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
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;

namespace hepce {
namespace testing {

class HIVTreatmentTest : public ::testing::Test {
protected:
    NiceMock<MockPerson> mock_person;

    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    data::LinkageDetails linkage = {data::LinkageState::kLinked, 0, 1};
    data::TreatmentDetails treatment = {true, 0, 1, 0, 0, 0, 0, false};
    data::HIVDetails hiv = {data::HIV::kLoUn, 0, 0};
    data::PregnancyDetails pregnancy = {
        data::PregnancyState::kNa, -1, 0, 0, 0, 0, 0, 0, {}};

    void SetUp() override {
        std::unordered_map<std::string, std::vector<std::string>> config =
            DEFAULT_CONFIG;
        config["eligibility"] = {
            "ineligible_drug_use =", "ineligible_fibrosis_stages =",
            "ineligible_pregnancy_states =",
            "ineligible_time_former_threshold =",
            "ineligible_time_since_linked ="};
        config["hiv_treatment"] = {"course = baseline"};

        BuildSimConf(test_conf, config);

        ExecuteQueries(test_db, {"DROP TABLE IF EXISTS hiv_treatments;",
                                 "CREATE TABLE hiv_treatments("
                                 "course TEXT NOT NULL,"
                                 "cost REAL NOT NULL,"
                                 "toxicity_prob REAL NOT NULL,"
                                 "withdrawal_prob REAL NOT NULL,"
                                 "months_to_suppression INTEGER NOT NULL,"
                                 "months_to_high_cd4 INTEGER NOT NULL);",
                                 "INSERT INTO hiv_treatments VALUES "
                                 "('baseline', 321.0, 0.0, 0.0, 2, 3);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS HIV_table;",
                        "CREATE TABLE HIV_table("
                        "HIV_trt TEXT NOT NULL,"
                        "CD4_count TEXT NOT NULL,"
                        "utility REAL NOT NULL);",
                        "INSERT INTO HIV_table VALUES ('OFF', 'high', 0.90);",
                        "INSERT INTO HIV_table VALUES ('OFF', 'low', 0.80);",
                        "INSERT INTO HIV_table VALUES ('ON', 'high', 0.82);",
                        "INSERT INTO HIV_table VALUES ('ON', 'low', 0.72);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS lost_to_follow_up;",
                        CreateLostToFollowUps(),
                        "INSERT INTO lost_to_follow_up VALUES (-1, 0.0);",
                        "INSERT INTO lost_to_follow_up VALUES (0, 0.0);",
                        "INSERT INTO lost_to_follow_up VALUES (1, 0.0);",
                        "INSERT INTO lost_to_follow_up VALUES (2, 0.0);",
                        "INSERT INTO lost_to_follow_up VALUES (3, 0.0);",
                        "INSERT INTO lost_to_follow_up VALUES (4, 0.0);"});

        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
        ON_CALL(mock_person, GetLinkageDetails(_))
            .WillByDefault(Return(linkage));
        ON_CALL(mock_person, GetTreatmentDetails(_))
            .WillByDefault(Return(treatment));
        ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
            return hiv;
        }));
        ON_CALL(mock_person, SetHIV(_))
            .WillByDefault(Invoke([this](data::HIV next) { hiv.hiv = next; }));
        ON_CALL(mock_person, GetPregnancyDetails())
            .WillByDefault(Return(pregnancy));
        ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(HIVTreatmentTest, ExecutesSuppressionPathWhenLinkedAndOnTreatment) {
    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVTreatmentExec");
    auto sampler = model::Sampler::Create(42, "HIVTreatmentExecSampler");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(AtLeast(1));
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kTreatment))
        .Times(AtLeast(1));

    event->Execute(mock_person, *sampler);
}

TEST_F(HIVTreatmentTest, DoesNotExecuteWhenPersonIsNotLinked) {
    linkage.link_state = data::LinkageState::kUnlinked;
    ON_CALL(mock_person, GetLinkageDetails(_)).WillByDefault(Return(linkage));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVTreatmentUnlinked");
    auto sampler = model::Sampler::Create(42, "HIVTreatmentUnlinkedSampler");
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetHIV(_)).Times(0);

    event->Execute(mock_person, *sampler);
}

TEST_F(HIVTreatmentTest, DoesNotExecuteWhenPersonIsNotAlive) {
    ON_CALL(mock_person, IsAlive()).WillByDefault(Return(false));

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HIVTreatment", inputs, "HIVDead");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, _)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, StopsWhenLostToFollowUp) {
    ExecuteQueries(test_db,
                   {"DELETE FROM lost_to_follow_up;",
                    "INSERT INTO lost_to_follow_up VALUES (-1, 1.0);"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVLostToFollowUp");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, Unlink(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, _)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, StopsWhenTreatmentHasNotStartedYet) {
    treatment.initiated_treatment = false;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVNotInitiated");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kTreatment))
        .Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, WithdrawalsDropSuppression) {
    hiv.hiv = data::HIV::kLoSu;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(1));

    ExecuteQueries(test_db, {"UPDATE hiv_treatments SET withdrawal_prob = 1.0, "
                             "toxicity_prob = 0.0 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVWithdraws");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, AddWithdrawal(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, SetHIV(data::HIV::kLoUn)).Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, SuppressionThenHighCd4Restoration) {
    hiv.hiv = data::HIV::kLoUn;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));
    ON_CALL(mock_person, GetCurrentTimestep()).WillByDefault(Return(2));

    treatment.time_of_treatment_initiation = 0;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    ExecuteQueries(test_db,
                   {"UPDATE hiv_treatments SET months_to_suppression = 2, "
                    "months_to_high_cd4 = 2, withdrawal_prob = 0.0, "
                    "toxicity_prob = 0.0 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVSuppressionAndCD4");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetHIV(data::HIV::kLoSu)).Times(1);
    EXPECT_CALL(mock_person, SetHIV(data::HIV::kHiSu)).Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, ToxicityAddsToxicCostsAndUtility) {
    ExecuteQueries(test_db,
                   {"UPDATE hiv_treatments SET toxicity_prob = 1.0, "
                    "withdrawal_prob = 0.0, months_to_suppression = 99, "
                    "months_to_high_cd4 = 99 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HIVTreatment", inputs, "HIVToxic");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddToxicReaction(data::InfectionType::kHiv))
        .Times(1);
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(AtLeast(3));
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.72), model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.21), model::UtilityCategory::kTreatment))
        .Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, HighUnsuppressedTransitionsToHighSuppressed) {
    hiv.hiv = data::HIV::kHiUn;
    treatment.time_of_treatment_initiation = 0;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    ExecuteQueries(test_db,
                   {"UPDATE hiv_treatments SET months_to_suppression = 2, "
                    "months_to_high_cd4 = 99, withdrawal_prob = 0.0, "
                    "toxicity_prob = 0.0 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVHighSuppression");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetHIV(data::HIV::kHiSu)).Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, WithdrawalFromHighSuppressedLosesSuppression) {
    hiv.hiv = data::HIV::kHiSu;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));

    ExecuteQueries(test_db, {"UPDATE hiv_treatments SET withdrawal_prob = 1.0, "
                             "toxicity_prob = 0.0 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVWithdrawHigh");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, SetHIV(data::HIV::kHiUn)).Times(1);
    EXPECT_CALL(mock_person, SetHIV(data::HIV::kHiSu)).Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.82), model::UtilityCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person,
                SetUtility(DoubleEq(0.90), model::UtilityCategory::kTreatment))
        .Times(1);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, HivNoneLeavesSuppressionStateUnchanged) {
    hiv.hiv = data::HIV::kNone;
    treatment.time_of_treatment_initiation = 0;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    ExecuteQueries(test_db,
                   {"UPDATE hiv_treatments SET months_to_suppression = 2, "
                    "months_to_high_cd4 = 2, withdrawal_prob = 0.0, "
                    "toxicity_prob = 0.0 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVNoneNoSuppression");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(1));
    EXPECT_CALL(mock_person, SetHIV(_)).Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, IneligiblePersonSkipsInitiationBranch) {
    treatment.initiated_treatment = false;
    ON_CALL(mock_person, GetTreatmentDetails(_))
        .WillByDefault(Return(treatment));

    std::unordered_map<std::string, std::vector<std::string>> config =
        DEFAULT_CONFIG;
    config["eligibility"] = {
        "ineligible_drug_use = injection",
        "ineligible_fibrosis_stages =", "ineligible_pregnancy_states =",
        "ineligible_time_former_threshold =", "ineligible_time_since_linked ="};
    config["hiv_treatment"] = {"course = baseline"};
    BuildSimConf(test_conf, config);

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVIneligibleInit");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_)).WillOnce(Return(1));
    EXPECT_CALL(mock_person, AddCost(_, _, model::CostCategory::kTreatment))
        .Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kTreatment))
        .Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, LostToFollowUpWithHivNoneHitsResetUtilityDefault) {
    hiv.hiv = data::HIV::kNone;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));
    ExecuteQueries(test_db,
                   {"DELETE FROM lost_to_follow_up;",
                    "INSERT INTO lost_to_follow_up VALUES (-1, 1.0);"});

    data::Inputs inputs(test_conf, test_db);
    auto event =
        event::EventFactory::CreateEvent("HIVTreatment", inputs, "HIVNoneLTFU");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(mock_person, EndTreatment(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, Unlink(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kTreatment))
        .Times(0);

    event->Execute(mock_person, sampler);
}

TEST_F(HIVTreatmentTest, WithdrawalWithHivNoneUsesLoseSuppressionDefault) {
    hiv.hiv = data::HIV::kNone;
    ON_CALL(mock_person, GetHIVDetails()).WillByDefault(Invoke([this]() {
        return hiv;
    }));

    ExecuteQueries(test_db,
                   {"UPDATE hiv_treatments SET withdrawal_prob = 1.0, "
                    "toxicity_prob = 0.0, months_to_suppression = 99, "
                    "months_to_high_cd4 = 99 WHERE course = 'baseline';"});

    data::Inputs inputs(test_conf, test_db);
    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "HIVNoneWithdrawDefault");
    MockSampler sampler;
    ASSERT_NE(event, nullptr);

    EXPECT_CALL(sampler, GetDecision(_))
        .WillOnce(Return(1))
        .WillOnce(Return(1))
        .WillOnce(Return(0));
    EXPECT_CALL(mock_person, AddWithdrawal(data::InfectionType::kHiv)).Times(1);
    EXPECT_CALL(mock_person, SetHIV(_)).Times(0);
    EXPECT_CALL(mock_person, SetUtility(_, model::UtilityCategory::kTreatment))
        .Times(0);

    event->Execute(mock_person, sampler);
}

} // namespace testing
} // namespace hepce

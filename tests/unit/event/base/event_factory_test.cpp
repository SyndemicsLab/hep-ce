////////////////////////////////////////////////////////////////////////////////
// File: event_factory_test.cpp                                               //
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

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

#include <config.hpp>
#include <inputs_db.hpp>

namespace hepce {
namespace testing {

class EventFactoryTest : public ::testing::Test {
protected:
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    void SetUp() override {
        std::unordered_map<std::string, std::vector<std::string>> config =
            DEFAULT_CONFIG;
        config["eligibility"] = {
            "ineligible_drug_use =", "ineligible_fibrosis_stages =",
            "ineligible_pregnancy_states =",
            "ineligible_time_former_threshold =",
            "ineligible_time_since_linked ="};
        config["hiv_treatment"] = {"course = baseline"};
        config["hiv_screening"] = {"intervention_type = one-time",
                                   "period = 12"};
        config["hiv_linking"] = {
            "intervention_cost = 0.0", "false_positive_test_cost = 0.0",
            "scaling_coefficient = 1.0", "recent_screen_cutoff = 3"};

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

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS background_impacts;",
                        CreateBackgroundImpacts(),
                        "INSERT INTO background_impacts VALUES (25, 0, 4, "
                        "0.821, 370.75);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS background_mortality;",
                        CreateBackgroundMortalities(),
                        "INSERT INTO background_mortality VALUES (25, 0, "
                        "0.001);"});

        ExecuteQueries(test_db, {"DROP TABLE IF EXISTS smr;", CreateSmrs(),
                                 "INSERT INTO smr VALUES (0, 4, 1.0);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS behavior_transitions;",
                        "DROP TABLE IF EXISTS behavior_impacts;",
                        CreateBehaviorImpacts(), CreateBehaviorTransitions(),
                        "INSERT INTO behavior_impacts VALUES (0, 4, 370.75, "
                        "0.574);",
                        "INSERT INTO behavior_transitions VALUES (25, 0, 4, "
                        "0, 0.0, 0.0, 0.002, 0.0, 0.998);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS hcv_impacts;", CreateHCVImpacts(),
                        "INSERT INTO hcv_impacts VALUES (0, 0, 230.65, "
                        "0.915);",
                        "INSERT INTO hcv_impacts VALUES (1, 0, 430.00, "
                        "0.8);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS fibrosis;", CreateFibrosis(),
                        "INSERT INTO fibrosis VALUES (0, 0, 0.4, 0.9, 0.2, "
                        "1, 0.6);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS incidence;", CreateIncidence(),
                        "INSERT INTO incidence VALUES (25, 0, 4, 0.0102);"});

        ExecuteQueries(
            test_db,
            {"DROP TABLE IF EXISTS hiv_incidence;",
             "CREATE TABLE hiv_incidence(age_years INTEGER NOT NULL, "
             "gender INTEGER NOT NULL, drug_behavior INTEGER NOT NULL, "
             "incidence REAL NOT NULL, PRIMARY KEY(age_years, gender, "
             "drug_behavior));",
             "INSERT INTO hiv_incidence VALUES (25, 0, 4, 0.0050);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS screening_and_linkage;",
                        CreateScreeningAndLinkage(),
                        "INSERT INTO screening_and_linkage VALUES (25, 0, 4, "
                        "-1, 0.02, 0.03, 1.0, 0.4);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS antibody_testing;",
                        CreateAntibodyTesting(),
                        "INSERT INTO antibody_testing VALUES (25, 4, 1.0);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS treatments;", CreateTreatments(),
                        "INSERT INTO treatments VALUES (0, 0, 0, 'gp', 2, "
                        "10, 1, 0, 0.004, 0);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS treatment_initiations;",
                        CreateTreatmentInitializations(),
                        "INSERT INTO treatment_initiations VALUES (-1, "
                        "0.92);",
                        "INSERT INTO treatment_initiations VALUES (0, "
                        "0.92);",
                        "INSERT INTO treatment_initiations VALUES (1, "
                        "0.92);",
                        "INSERT INTO treatment_initiations VALUES (2, "
                        "0.92);",
                        "INSERT INTO treatment_initiations VALUES (3, "
                        "0.92);",
                        "INSERT INTO treatment_initiations VALUES (4, "
                        "0.92);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS overdoses;", CreateOverdoses(),
                        "INSERT INTO overdoses VALUES (-1, 0, 4, 0.5, 10.00, "
                        "0.2);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS pregnancy;", CreatePregnancy(),
                        "INSERT INTO pregnancy VALUES (25, 0.008, 0.0005);"});

        ExecuteQueries(
            test_db,
            {"DROP TABLE IF EXISTS moud_transitions;", CreateMoudTransitions(),
             "INSERT INTO moud_transitions VALUES (25, 0, 0, -1, 1.0, 0.0, "
             "0.0);",
             "INSERT INTO moud_transitions VALUES (25, 1, 4, -1, 0.0, 1.0, "
             "0.0);",
             "INSERT INTO moud_transitions VALUES (25, 2, 4, -1, 0.0, 1.0, "
             "0.0);"});

        ExecuteQueries(test_db,
                       {"DROP TABLE IF EXISTS moud_costs;",
                        "CREATE TABLE moud_costs(moud INTEGER NOT NULL, "
                        "pregnancy INTEGER NOT NULL, cost REAL NOT NULL, "
                        "utility REAL NOT NULL, PRIMARY KEY(moud, "
                        "pregnancy));",
                        "INSERT INTO moud_costs VALUES (0, -1, 0.0, 1.0);",
                        "INSERT INTO moud_costs VALUES (1, -1, 1.0, 0.99);",
                        "INSERT INTO moud_costs VALUES (2, -1, 0.5, 0.995);"});
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }
};

TEST_F(EventFactoryTest, ReturnsNullForUnknownEventName) {
    data::Inputs inputs(test_conf, test_db);

    auto event = event::EventFactory::CreateEvent("NotAnEvent", inputs,
                                                  "EventFactoryUnknown");

    EXPECT_EQ(event, nullptr);
}

TEST_F(EventFactoryTest, CreatesHIVTreatmentEvent) {
    data::Inputs inputs(test_conf, test_db);

    auto event = event::EventFactory::CreateEvent("HIVTreatment", inputs,
                                                  "EventFactoryHivTreatment");

    EXPECT_NE(event, nullptr);
}

TEST_F(EventFactoryTest, CreatesDeathEvent) {
    data::Inputs inputs(test_conf, test_db);

    auto event =
        event::EventFactory::CreateEvent("Death", inputs, "EventFactoryDeath");

    EXPECT_NE(event, nullptr);
}

TEST_F(EventFactoryTest, CreatesVoluntaryRelinkingEvent) {
    data::Inputs inputs(test_conf, test_db);

    auto event = event::EventFactory::CreateEvent(
        "VoluntaryRelinking", inputs, "EventFactoryVoluntaryRelinking");

    EXPECT_NE(event, nullptr);
}

TEST_F(EventFactoryTest, CreatesAllKnownFactoryEvents) {
    data::Inputs inputs(test_conf, test_db);

    const std::vector<std::string> event_names = {"Aging",
                                                  "BehaviorChanges",
                                                  "Clearance",
                                                  "Death",
                                                  "FibrosisProgression",
                                                  "FibrosisStaging",
                                                  "HCVInfection",
                                                  "HCVLinking",
                                                  "HCVScreening",
                                                  "HCVTreatment",
                                                  "HIVInfection",
                                                  "HIVLinking",
                                                  "HIVScreening",
                                                  "HIVTreatment",
                                                  "Overdose",
                                                  "VoluntaryRelinking",
                                                  "Pregnancy",
                                                  "MOUD"};

    for (const auto &event_name : event_names) {
        SCOPED_TRACE(event_name);
        auto created =
            event::EventFactory::CreateEvent(event_name, inputs, "FactoryAll");
        EXPECT_NE(created, nullptr);

        auto cloned = created->clone();
        EXPECT_NE(cloned, nullptr);
        EXPECT_NE(cloned.get(), created.get());
    }
}

TEST_F(EventFactoryTest, ReturnsNullForFallbackVariants) {
    data::Inputs inputs(test_conf, test_db);

    EXPECT_EQ(event::EventFactory::CreateEvent("", inputs, "FactoryEmpty"),
              nullptr);
    EXPECT_EQ(event::EventFactory::CreateEvent("hivtreatment", inputs,
                                               "FactoryCaseMismatch"),
              nullptr);
    EXPECT_EQ(event::EventFactory::CreateEvent(" HIVTreatment", inputs,
                                               "FactoryWhitespacePrefix"),
              nullptr);
}

} // namespace testing
} // namespace hepce

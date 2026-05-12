////////////////////////////////////////////////////////////////////////////////
// File: simulation_test.cpp                                                  //
// Project: hep-ce                                                            //
// Created Date: 2023-09-13                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/model/simulation.hpp>

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include <config.hpp>
#include <inputs_db.hpp>

// 3rd Party Dependencies
#include <gtest/gtest.h>

class SimulationTest : public ::testing::Test {
protected:
    std::string test_db = "inputs.db";
    std::string test_conf = "sim.conf";

    void SetUp() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }

    void TearDown() override {
        std::filesystem::remove(test_db);
        std::filesystem::remove(test_conf);
    }

    hepce::data::Inputs
    BuildInputs(const std::vector<std::string> &simulation_overrides) {
        auto config = hepce::testing::DEFAULT_CONFIG;
        config["simulation"] = simulation_overrides;
        hepce::testing::BuildSimConf(test_conf, config);
        return hepce::data::Inputs(test_conf, test_db);
    }
};

TEST_F(SimulationTest, CreateSetsDurationAndConfiguredSeed) {
    auto inputs = BuildInputs(
        {"seed = 123", "population_size = 1", "events = NotAnEvent",
         "duration = 7", "start_time = 0", "use_population_table = false"});

    auto sim = hepce::model::Hepce::Create(inputs, "SimCtor");

    ASSERT_NE(sim, nullptr);
    EXPECT_EQ(sim->GetDuration(), 7);
    EXPECT_EQ(sim->GetSeed(), 123);
}

TEST_F(SimulationTest, CreateUsesGeneratedSeedWhenConfiguredSeedIsNegative) {
    auto inputs = BuildInputs(
        {"seed = -5", "population_size = 1", "events = NotAnEvent",
         "duration = 5", "start_time = 0", "use_population_table = false"});

    auto sim = hepce::model::Hepce::Create(inputs, "SimNegativeSeed");

    ASSERT_NE(sim, nullptr);
    EXPECT_EQ(sim->GetDuration(), 5);
    EXPECT_GE(sim->GetSeed(), 0);
}

TEST_F(SimulationTest, CreateEventsIncludesNullForUnknownEventName) {
    auto inputs = BuildInputs(
        {"seed = 42", "population_size = 1", "events = NotAnEvent",
         "duration = 1", "start_time = 0", "use_population_table = false"});

    auto sim = hepce::model::Hepce::Create(inputs, "SimEvents");
    auto events = sim->CreateEvents();

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0], nullptr);
}

TEST_F(SimulationTest, RunNoopsWithEmptyPopulationAndEvents) {
    auto inputs = BuildInputs(
        {"seed = 21", "population_size = 0", "events = NotAnEvent",
         "duration = 1", "start_time = 0", "use_population_table = false"});

    auto sim = hepce::model::Hepce::Create(inputs, "SimRun");
    hepce::model::People people;
    hepce::event::EventList events;

    EXPECT_NO_THROW(sim->Run(people, events));
}

TEST_F(SimulationTest, CreatePopulationFromInitCohortTableBranch) {
    auto inputs = BuildInputs(
        {"seed = 11", "population_size = 1", "events = NotAnEvent",
         "duration = 1", "start_time = 0", "use_population_table = false"});

    hepce::testing::ExecuteQueries(
        test_db,
        {"DROP TABLE IF EXISTS init_cohort;",
         "CREATE TABLE init_cohort(id INTEGER PRIMARY KEY, age_months INTEGER, "
         "gender INTEGER, drug_behavior INTEGER, time_last_active_drug_use "
         "INTEGER, seropositivity INTEGER, genotype_three INTEGER, "
         "fibrosis_state INTEGER, identified_as_hcv_positive INTEGER, "
         "link_state INTEGER, hcv_status INTEGER, pregnancy_state INTEGER);",
         "INSERT INTO init_cohort VALUES (1, 300, 0, 4, -1, 0, 0, 0, 0, 0, "
         "0, -1);"});

    auto sim = hepce::model::Hepce::Create(inputs, "SimPopulation");
    auto population = sim->CreatePopulation();

    ASSERT_EQ(population.size(), 1);
    EXPECT_NE(population[0], nullptr);
    EXPECT_EQ(population[0]->GetAge(), 300);
}

TEST_F(SimulationTest, DeepRunExecutesAgingEventAcrossDuration) {
    auto inputs = BuildInputs(
        {"seed = 77", "population_size = 1", "events = Aging", "duration = 2",
         "start_time = 0", "use_population_table = false"});

    hepce::testing::ExecuteQueries(
        test_db,
        {"DROP TABLE IF EXISTS init_cohort;",
         "CREATE TABLE init_cohort(id INTEGER PRIMARY KEY, age_months INTEGER, "
         "gender INTEGER, drug_behavior INTEGER, time_last_active_drug_use "
         "INTEGER, seropositivity INTEGER, genotype_three INTEGER, "
         "fibrosis_state INTEGER, identified_as_hcv_positive INTEGER, "
         "link_state INTEGER, hcv_status INTEGER, pregnancy_state INTEGER);",
         "INSERT INTO init_cohort VALUES (1, 300, 0, 4, -1, 0, 0, 0, 0, 0, "
         "0, -1);",
         "DROP TABLE IF EXISTS background_impacts;",
         hepce::testing::CreateBackgroundImpacts(),
         "INSERT INTO background_impacts VALUES (25, 0, 4, 0.821, 370.75);"});

    auto sim = hepce::model::Hepce::Create(inputs, "DeepSimRun");
    auto events = sim->CreateEvents();
    auto population = sim->CreatePopulation();

    ASSERT_EQ(events.size(), 1);
    ASSERT_NE(events[0], nullptr);
    ASSERT_EQ(population.size(), 1);
    ASSERT_NE(population[0], nullptr);

    sim->Run(population, events);

    EXPECT_EQ(population[0]->GetCurrentTimestep(), 2);
    EXPECT_EQ(population[0]->GetAge(), 302);
}

TEST_F(SimulationTest, DeepPopulationTableMissingReturnsEmptyPopulation) {
    auto inputs = BuildInputs(
        {"seed = 19", "population_size = 3", "events = NotAnEvent",
         "duration = 1", "start_time = 0", "use_population_table = true"});

    auto sim = hepce::model::Hepce::Create(inputs, "DeepPopTable");
    auto population = sim->CreatePopulation();

    EXPECT_TRUE(population.empty());
}

TEST_F(SimulationTest, CloneProducesEquivalentSimulationSettings) {
    auto inputs = BuildInputs(
        {"seed = 222", "population_size = 1", "events = NotAnEvent",
         "duration = 9", "start_time = 0", "use_population_table = false"});

    auto sim = hepce::model::Hepce::Create(inputs, "CloneableSim");
    auto cloned = sim->clone();

    ASSERT_NE(sim, nullptr);
    ASSERT_NE(cloned, nullptr);
    EXPECT_NE(sim.get(), cloned.get());
    EXPECT_EQ(cloned->GetDuration(), sim->GetDuration());
    EXPECT_EQ(cloned->GetSeed(), sim->GetSeed());
}

TEST_F(SimulationTest, DeepPopulationTableSuccessExecutesPopulationCallback) {
    auto inputs = BuildInputs({
        "seed = 88",
        "population_size = 1",
        "events = pregnancy,HCCScreening,BehaviorChanges,HIVInfections,MOUD",
        "duration = 1",
        "start_time = 2",
        "use_population_table = true",
    });

    const std::string headers =
        hepce::data::POPULATION_HEADERS(true, true, true, true, true);
    std::stringstream header_ss(headers);
    std::string col;
    std::vector<std::string> cols;
    while (std::getline(header_ss, col, ',')) {
        if (!col.empty()) {
            cols.push_back(col);
        }
    }
    ASSERT_FALSE(cols.empty());

    std::vector<std::string> values(cols.size(), "0");
    values[0] = "1";   // sex
    values[1] = "456"; // age
    values[5] = "4";   // behavior
    values[7] = "2";   // hcv
    values[8] = "4";   // fibrosis
    values[16] = "3";  // hiv
    values[23] = "1";  // moud
    values[27] = "1";  // pregnancy
    values[35] = "2";  // measured fibrosis
    values[38] = "1";  // hcv link state
    values[39] = "77"; // hcv link time

    std::stringstream create_table;
    create_table << "CREATE TABLE population(id INTEGER PRIMARY KEY";
    for (const auto &name : cols) {
        create_table << "," << name;
    }
    create_table << ");";

    std::stringstream insert_row;
    insert_row << "INSERT INTO population(id";
    for (const auto &name : cols) {
        insert_row << "," << name;
    }
    insert_row << ") VALUES (1";
    for (const auto &value : values) {
        insert_row << "," << value;
    }
    insert_row << ");";

    hepce::testing::ExecuteQueries(test_db,
                                   {"DROP TABLE IF EXISTS population;",
                                    create_table.str(), insert_row.str()});

    auto sim = hepce::model::Hepce::Create(inputs, "DeepPopTableSuccess");
    auto population = sim->CreatePopulation();

    ASSERT_EQ(population.size(), 1);
    ASSERT_NE(population[0], nullptr);
    EXPECT_EQ(population[0]->GetAge(), 456);
    EXPECT_EQ(population[0]->GetSex(), hepce::data::Sex::kFemale);
    EXPECT_EQ(population[0]->GetBehaviorDetails().behavior,
              hepce::data::Behavior::kInjection);
    EXPECT_EQ(population[0]->GetHCVDetails().hcv, hepce::data::HCV::kChronic);
    EXPECT_EQ(population[0]->GetHCVDetails().fibrosis_state,
              hepce::data::FibrosisState::kF4);
    EXPECT_EQ(population[0]->GetHIVDetails().hiv, hepce::data::HIV::kLoUn);
    EXPECT_EQ(population[0]->GetMoudDetails().moud_state,
              hepce::data::MOUD::kCurrent);
    EXPECT_EQ(population[0]->GetPregnancyDetails().pregnancy_state,
              hepce::data::PregnancyState::kPregnant);
}

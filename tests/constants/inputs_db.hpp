////////////////////////////////////////////////////////////////////////////////
// File: inputs_db.hpp                                                        //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_TESTS_CONSTANTS_INPUTSDB_HPP_
#define HEPCE_TESTS_CONSTANTS_INPUTSDB_HPP_

#include <iostream>
#include <vector>

#include <SQLiteCpp/SQLiteCpp.h>

namespace hepce {
namespace testing {

inline int ExecuteQueries(const std::string &db_name,
                          const std::vector<std::string> &queries) {
    try {
        SQLite::Database db(db_name,
                            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        SQLite::Transaction transaction(db);
        for (const std::string &query : queries) {
            db.exec(query);
        }
        transaction.commit();
        return EXIT_SUCCESS;
    } catch (std::exception &e) {
        std::cout << "SQLite Exception in Creating Inputs Database: "
                  << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

inline const std::string CreateBackgroundImpacts() {
    std::stringstream s;
    s << ("CREATE TABLE background_impacts("
          "age_years INTEGER NOT NULL, "
          "gender INTEGER NOT NULL, "
          "drug_behavior INTEGER NOT NULL, "
          "utility REAL NOT NULL, "
          "cost REAL NOT NULL DEFAULT 0.0, "
          "PRIMARY KEY(age_years, gender, drug_behavior));");
    return s.str();
}

inline const std::string CreateBackgroundMortalities() {
    std::stringstream s;
    s << ("CREATE TABLE background_mortality (age_years INTEGER NOT NULL, "
          "gender INTEGER NOT NULL, background_mortality REAL NOT NULL, "
          "PRIMARY KEY(age_years, gender));");
    return s.str();
}

inline const std::string CreateOverdoses() {
    std::stringstream s;
    s << ("CREATE TABLE overdoses "
        "(moud INTEGER, drug_behavior INTEGER, "
        "fatality_probability REAL);");
    return s.str();
}

inline const std::string CreateSmrs() {
    std::stringstream s;
    s << ("CREATE TABLE smr (gender	INTEGER NOT NULL,drug_behavior "
          "INTEGER NOT NULL,smr	REAL NOT NULL, PRIMARY "
          "KEY(gender,drug_behavior));");
    return s.str();
}

inline const std::string CreateBehaviorImpacts() {
    std::stringstream s;
    s << "CREATE TABLE behavior_impacts (gender INTEGER NOT NULL, "
         "drug_behavior	INTEGER NOT NULL, cost REAL NOT NULL DEFAULT 0.0, "
         "utility INTEGER NOT NULL, PRIMARY KEY(gender,drug_behavior));";
    return s.str();
}

inline const std::string CreateBehaviorTransitions() {
    std::stringstream s;
    s << "CREATE TABLE behavior_transitions (age_years	INTEGER NOT "
         "NULL,gender	INTEGER NOT NULL,drug_behavior	INTEGER NOT "
         "NULL,moud	INTEGER NOT NULL,never	REAL NOT NULL DEFAULT "
         "0.0,former_noninjection	REAL NOT NULL DEFAULT "
         "0.0,former_injection	REAL NOT NULL DEFAULT 0.0,noninjection	REAL "
         "NOT NULL DEFAULT 0.0,injection	REAL NOT NULL DEFAULT "
         "0.0,PRIMARY KEY(age_years,gender,drug_behavior,moud));";
    return s.str();
}

inline const std::string CreatePregnancy() {
    std::stringstream s;
    s << "CREATE TABLE pregnancy (age_years	INTEGER NOT NULL "
         "UNIQUE,pregnancy_probability	REAL NOT NULL,stillbirth	"
         "REAL,PRIMARY KEY(age_years));";
    return s.str();
}

inline const std::string CreateHCVImpacts() {
    std::stringstream s;
    s << "CREATE TABLE hcv_impacts (hcv_status	INTEGER NOT NULL DEFAULT "
         "0,fibrosis_state	INTEGER NOT NULL,cost	REAL NOT NULL DEFAULT "
         "0.0,utility	REAL NOT NULL,PRIMARY KEY(hcv_status,fibrosis_state));";
    return s.str();
}

inline const std::string CreateFibrosis() {
    std::stringstream s;
    s << "CREATE TABLE fibrosis (fibrosis_state	INTEGER NOT "
         "NULL,diagnosed_fibrosis	INTEGER NOT NULL,apri	REAL NOT "
         "NULL,fibroscan	REAL NOT NULL,fibrotest	REAL NOT "
         "NULL,idealtest	REAL NOT NULL,fib4	REAL NOT NULL,PRIMARY "
         "KEY(fibrosis_state,diagnosed_fibrosis));";
    return s.str();
}

inline const std::string CreateIncidence() {
    std::stringstream s;
    s << "CREATE TABLE incidence (age_years	INTEGER NOT NULL,gender	"
         "INTEGER NOT NULL,drug_behavior	INTEGER NOT "
         "NULL,incidence	REAL NOT NULL,PRIMARY "
         "KEY(age_years,gender,drug_behavior),FOREIGN KEY(drug_behavior) "
         "REFERENCES drug_behaviors(id));";
    return s.str();
}

inline const std::string CreateScreeningAndLinkage() {
    std::stringstream s;
    s << "CREATE TABLE screening_and_linkage (age_years INTEGER NOT "
         "NULL,gender INTEGER NOT NULL,drug_behavior INTEGER NOT "
         "NULL,pregnancy INTEGER NOT NULL DEFAULT "
         "-1,background_screen_probability REAL NOT "
         "NULL,background_link_probability REAL NOT "
         "NULL,intervention_screen_probability REAL NOT "
         "NULL,intervention_link_probability REAL NOT NULL,PRIMARY "
         "KEY(age_years, gender, drug_behavior, pregnancy));";
    return s.str();
}

inline const std::string CreateAntibodyTesting() {
    std::stringstream s;
    s << "CREATE TABLE antibody_testing (age_years	INTEGER NOT "
         "NULL,drug_behavior	INTEGER NOT NULL,accept_probability	REAL "
         "NOT NULL,PRIMARY KEY(age_years,drug_behavior));";
    return s.str();
}

inline const std::string CreateTreatments() {
    std::stringstream s;
    s << "CREATE TABLE treatments(salvage INT,genotype_three INT,cirrhotic "
         "INT,course TEXT,duration INT,cost REAL,svr_prob_if_completed "
         "REAL,toxicity_prob_if_withdrawal REAL,withdrawal REAL,toxicity_prob "
         "REAL);";
    return s.str();
}

inline const std::string CreateTreatmentInitializations() {
    std::stringstream s;
    s << "CREATE TABLE treatment_initiations(pregnancy_state INT, "
         "treatment_initiation REAL);";
    return s.str();
}

inline const std::string CreateLostToFollowUps() {
    std::stringstream s;
    s << "CREATE TABLE lost_to_follow_up(pregnancy_state INTEGER NOT NULL "
         "UNIQUE, probability REAL NOT NULL, PRIMARY "
         "KEY(pregnancy_state));";
    return s.str();
}

} // namespace testing
} // namespace hepce

#endif // HEPCE_TESTS_CONSTANTS_INPUTSDB_HPP_
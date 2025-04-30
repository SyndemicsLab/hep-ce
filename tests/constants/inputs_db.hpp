////////////////////////////////////////////////////////////////////////////////
// File: inputs_db.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
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

int ExecuteQueries(const std::string &db_name,
                   const std::vector<std::string> &queries) {
    try {
        SQLite::Database db(db_name,
                            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        db.exec("VACUUM;");
        for (const std::string &query : queries) {
            db.exec(query);
        }
        return EXIT_SUCCESS;
    } catch (std::exception &e) {
        std::cout << "SQLite Exception in Creating Inputs Database: "
                  << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

const std::string CreateBackgroundImpacts() {
    std::stringstream s;
    s << ("CREATE TABLE \"background_impacts\" (\"age_years\"	INTEGER NOT "
          "NULL, \"gender\"	INTEGER NOT NULL, \"drug_behavior\"	"
          "INTEGER NOT NULL, \"utility\"	REAL NOT NULL, \"cost\"	REAL "
          "NOT NULL DEFAULT 0.0, PRIMARY "
          "KEY(\"age_years\",\"gender\",\"drug_behavior\"), FOREIGN "
          "KEY(\"drug_behavior\") REFERENCES \"drug_behaviors\"(\"id\"), "
          "FOREIGN KEY(\"gender\") REFERENCES \"sex\"(\"id\")");
    return s.str();
}
} // namespace testing
} // namespace hepce

#endif // HEPCE_TESTS_CONSTANTS_INPUTSDB_HPP_
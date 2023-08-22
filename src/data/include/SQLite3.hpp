//===-- SQLite3.hpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef SQLITE3_HPP
#define SQLITE3_HPP

#include <memory>
#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

struct SQLTable {
    SQLTable() {}
    ~SQLTable() {
        sqlite3_free_table(this->data);
    }
    int nRows;
    int nCols;
    char **data;
    std::string name;
};

class Database {
public:
    /// @brief Constructor for the class that handles SQLite3 interfacing
    /// @param database
    Database(std::string database);
    ~Database();

    /// @brief
    /// @param
    SQLTable readTable(std::string query);

private:
    // pointer for the sqlite3 connection
    sqlite3 *db;
};
#endif // SQLITE3_HPP

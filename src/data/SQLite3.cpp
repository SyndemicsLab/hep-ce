#include <SQLite3.hpp>

namespace Data {
    Database::Database(std::string database) {
        // evaluates as true if an error is raised
        if (sqlite3_open_v2(database.c_str(), &this->db, SQLITE_OPEN_READONLY,
                            NULL)) {
            // add error handling here
            return;
        }
    }

    SQLTable Database::readTable(std::string query) {
        SQLTable toReturn;

        // evaluates as true if an error is raised
        if (sqlite3_get_table(this->db, query.c_str(), &toReturn.data,
                              &toReturn.nRows, &toReturn.nCols, NULL)) {
            // add error handling here
        }

        return toReturn;
    }
} // namespace Data

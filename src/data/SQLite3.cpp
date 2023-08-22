#include <SQLite3.hpp>

Database::Database(std::string database) {
    // evalutates as true if an error is raised
    if(sqlite3_open_v2(database.c_str(), &this->db, SQLITE_OPEN_READONLY, NULL)){
        // add error handling here
        return;
    }
}

Database::~Database() {
    sqlite3_close(this->db);
}

SQLTable Database::readTable(std::string query) {
    SQLTable toReturn;

    if(sqlite3_get_table(this->db, query.c_str(), &toReturn.data,
                         &toReturn.nRows, &toReturn.nCols, NULL)){
        // add error handling here
    }

    int row;
    for (int i = 0; i < toReturn.nRows; ++i) {
        row = i * toReturn.nCols;
        for (int j = 0; j < toReturn.nCols; ++j) {
            std::cout << row + j << ": " << toReturn.data[row + j] << std::endl;
        }
    }
    return toReturn;
}

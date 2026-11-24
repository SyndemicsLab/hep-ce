////////////////////////////////////////////////////////////////////////////////
// File: inputs.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2026-03-19                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-03                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_INPUTS_HPP_
#define HEPCE_DATA_INPUTS_HPP_

#include <any>
#include <filesystem>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include <SQLiteCpp/SQLiteCpp.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace hepce {
namespace data {
/// Provides access to the simulation configuration and SQLite input database.
class Inputs {
public:
    /// Loads the INI configuration and records the database path.
    /// @param config_file Path to the INI configuration file.
    /// @param database_file Path to the SQLite database file.
    Inputs(const std::string &config_file, const std::string &database_file)
        : _config_file(config_file), _database_file(database_file) {
        read_ini(_config_file.string(), _ptree);
    }

    ~Inputs() = default;

    // No rule of 3 or 5 needed because this is a complete and copy-able object

    /// Returns the parsed configuration property tree.
    const boost::property_tree::ptree &GetPropertyTree() const {
        return _ptree;
    }
    /// Executes a query and invokes a callback for each returned row.
    /// @param query SQL query to execute.
    /// @param callback Function called for each result row.
    /// @param storage Mutable caller-owned storage passed to the callback.
    /// @param bindings Positional SQL bindings keyed by parameter index.
    /// @throws std::runtime_error if the query or transaction fails.
    void SelectFromDatabase(
        const std::string &query,
        std::function<void(std::any &storage, const SQLite::Statement &stmt)>
            callback,
        std::any &storage,
        const std::unordered_map<int, std::variant<int, double, std::string>>
            &bindings) const {
        try {
            SQLite::Database db(_database_file.string(),
                                SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            SQLite::Statement stmt(db, query);

            for (const auto &[index, value] : bindings) {
                if (value.index() == 0) {
                    stmt.bind(index, std::get<int>(value));
                } else if (value.index() == 1) {
                    stmt.bind(index, std::get<double>(value));
                } else {
                    stmt.bind(index, std::get<std::string>(value));
                }
            }

            SQLite::Transaction transaction(db);

            while (stmt.executeStep()) {
                callback(storage, stmt);
            }

            transaction.commit();
        } catch (const std::exception &e) {
            throw std::runtime_error("Error executing query: " + query + "\n" +
                                     e.what());
        }
    }

private:
    const std::filesystem::path _config_file;
    const std::filesystem::path _database_file;
    boost::property_tree::ptree _ptree;
};

} // namespace data
} // namespace hepce

#endif // HEPCE_DATA_INPUTS_HPP_
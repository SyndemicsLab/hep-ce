////////////////////////////////////////////////////////////////////////////////
// File: inputs.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2026-03-19                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
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
class Inputs {
public:
    Inputs(const std::string &config_file, const std::string &database_file)
        : _config_file(config_file), _database_file(database_file) {
        read_ini(_config_file.string(), _ptree);
    }

    ~Inputs() = default;

    // No rule of 3 or 5 needed because this is a complete and copy-able object

    const boost::property_tree::ptree &GetPropertyTree() const {
        return _ptree;
    }
    void SelectFromDatabase(
        const std::string &query,
        std::function<void(std::any &storage, const SQLite::Statement &stmt)>
            callback,
        std::any &storage,
        const std::unordered_map<int, std::variant<int, double, std::string>>
            &bindings) {
        try {
            SQLite::Database db(_database_file,
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
    void ValidateFiles() {
        if (!std::filesystem::exists(_config_file)) {
            std::ostringstream msg;
            msg << "File `" << _config_file << "` not found!";
            throw std::runtime_error(msg.str());
        }
        if (!std::filesystem::exists(_database_file)) {
            std::ostringstream msg;
            msg << "File `" << _database_file << "` not found!";
            throw std::runtime_error(msg.str());
        }
    }

    const std::filesystem::path _config_file;
    const std::filesystem::path _database_file;
    boost::property_tree::ptree _ptree;
};

} // namespace data
} // namespace hepce

#endif // HEPCE_DATA_INPUTS_HPP_
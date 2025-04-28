////////////////////////////////////////////////////////////////////////////////
// File: DataManagerMock.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATAMANAGERMOCK_HPP_
#define DATAMANAGERMOCK_HPP_

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <datamanagement/datamanagement.hpp>
#include <string>
#include <vector>

namespace datamanagement {
class MOCKDataManager : public DataManagerBase {
public:
    using callbackfuncdef = int (*)(void *, int, char **, char **);
    MOCK_METHOD(int, AddCSVTable, (std::string const &filepath),
                (const, override));
    MOCK_METHOD(int, WriteTableToCSV,
                (std::string const &filepath, std::string tablename,
                 std::string column_names),
                (const, override));
    MOCK_METHOD(int, Create, (std::string const query, Table &data),
                (const, override));
    MOCK_METHOD(int, Select, (std::string const query, Table &data),
                (const, override));
    MOCK_METHOD(int, Update, (std::string const query, Table &data),
                (const, override));
    MOCK_METHOD(int, Delete, (std::string const query, Table &data),
                (const, override));
    MOCK_METHOD(int, SelectCustomCallback,
                (std::string const query, callbackfuncdef, void *data,
                 std::string &error),
                (const, override));
    MOCK_METHOD(int, LoadConfig, (std::string const &filepath), (override));
    MOCK_METHOD(std::string, GetConfigFile, (), (const, override));
    MOCK_METHOD(int, GetFromConfig, (std::string const key, std::string &data),
                (const, override));
    MOCK_METHOD(int, GetConfigSectionCategories,
                (std::string const section, std::vector<std::string> &data),
                (const, override));
    MOCK_METHOD(int, SaveDatabase, (std::string const &outfile), (override));
    MOCK_METHOD(int, StartTransaction, (), (const, override));
    MOCK_METHOD(int, EndTransaction, (), (const, override));
    MOCK_METHOD(std::string, GetDBFileName, (), (const, override));
    MOCK_METHOD(int, ConnectToDatabase, (std::string const &dbfile),
                (override));
    MOCK_METHOD(int, CloseConnection, (), (override));
};
} // namespace datamanagement

#endif

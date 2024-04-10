#ifndef TESTS_MOCKDATATABLE_HPP_
#define TESTS_MOCKDATATABLE_HPP_

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <DataTable.hpp>

class MockDataTable : public Data::IDataTable {
public:
    MockDataTable() {}
    virtual ~MockDataTable() = default;

    virtual std::shared_ptr<IDataTable>
    operator+(IDataTable const &tableTwo) const override {
        return concat(tableTwo);
    };

    virtual std::shared_ptr<IDataTable> operator[](int idx) const {
        return getRow(idx);
    };

    virtual std::vector<std::string> operator[](std::string columnName) const {
        return getColumn(columnName);
    };

    using IDataTablePtr = std::shared_ptr<Data::IDataTable>;
    using vecOfStr = std::vector<std::string>;
    using vecOfInt = std::vector<int>;
    using doubleVecOfStr = std::vector<std::vector<std::string>>;
    using unorderedMapStrToStr = std::unordered_map<std::string, std::string>;
    using mapStrToVecOfStr = std::map<std::string, std::vector<std::string>>;

    MOCK_METHOD(void, toCSV, (const std::string &), (const, override));
    MOCK_METHOD(bool, fromCSV, (const std::string &, bool, char), (override));
    MOCK_METHOD(bool, fromSQL, (const std::string &, const std::string &),
                (override));
    MOCK_METHOD(doubleVecOfStr, getData, (), (const, override));
    MOCK_METHOD(bool, checkColumnExists, (std::string), (const, override));
    MOCK_METHOD(mapStrToVecOfStr, getDataAsMap, (), (const, override));
    MOCK_METHOD(IDataTablePtr, getRow, (int), (const, override));
    MOCK_METHOD(vecOfStr, getColumn, (std::string), (const, override));
    MOCK_METHOD(vecOfStr, getColumnNames, (), (const, override));
    MOCK_METHOD(IDataTablePtr, selectColumns, (vecOfStr), (const, override));
    MOCK_METHOD(IDataTablePtr, selectRows, (vecOfInt), (const, override));
    MOCK_METHOD(IDataTablePtr, selectRowRange, (int, int), (const, override));
    MOCK_METHOD(IDataTablePtr, selectWhere, (unorderedMapStrToStr),
                (const, override));
    MOCK_METHOD(IDataTablePtr, innerJoin,
                (IDataTablePtr const, std::string, std::string),
                (const, override));
    MOCK_METHOD(IDataTablePtr, innerJoin,
                (IDataTablePtr const, vecOfStr, vecOfStr), (const, override));
    MOCK_METHOD(IDataTablePtr, leftJoin,
                (IDataTablePtr const, std::string, std::string),
                (const, override));
    MOCK_METHOD(IDataTablePtr, rightJoin,
                (IDataTablePtr const, std::string, std::string),
                (const, override));
    MOCK_METHOD(IDataTablePtr, outerJoin,
                (IDataTablePtr const, std::string, std::string),
                (const, override));
    MOCK_METHOD(IDataTablePtr, topNRows, (int), (const, override));
    MOCK_METHOD(IDataTablePtr, bottomNRows, (int), (const, override));
    MOCK_METHOD(IDataTablePtr, head, (), (const, override));
    MOCK_METHOD(IDataTablePtr, tail, (), (const, override));
    MOCK_METHOD(vecOfStr, getHeaders, (), (const, override));
    MOCK_METHOD(void, dropColumns, (vecOfStr), (override));
    MOCK_METHOD(void, dropColumn, (std::string), (override));
    MOCK_METHOD(void, shuffleRows, (int), (override));
    MOCK_METHOD(std::string, min, (std::string), (const, override));
    MOCK_METHOD(std::string, max, (std::string), (const, override));
    MOCK_METHOD(std::string, min, (), (const, override));
    MOCK_METHOD(std::string, max, (), (const, override));
    MOCK_METHOD(double, sum, (std::string), (const, override));
    MOCK_METHOD(double, sum, (), (const, override));
    MOCK_METHOD(double, mean, (std::string), (const, override));
    MOCK_METHOD(double, mean, (), (const, override));
    MOCK_METHOD(IDataTablePtr, concat, (IDataTable const &), (const, override));
    MOCK_METHOD(int, nrows, (), (const, override));
    MOCK_METHOD(int, ncols, (), (const, override));
    MOCK_METHOD(Data::DataTableShape, getShape, (), (const, override));
    MOCK_METHOD(bool, empty, (), (const, override));
};

#endif

////////////////////////////////////////////////////////////////////////////////
// File: writer_test.cpp                                                      //
// Project: hep-ce                                                            //
// Created: 2025-03-12                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: GitHub Copilot                                                //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/data/writer.hpp>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <gtest/gtest.h>

#include <person_mock.hpp>

using ::testing::NiceMock;
using ::testing::Return;

using namespace hepce::data;
using namespace hepce::model;

namespace hepce {
namespace testing {

class DataWriterTest : public ::testing::Test {
protected:
    std::filesystem::path test_dir =
        std::filesystem::path("build/test_artifacts/writer_tests");

    void SetUp() override {
        std::filesystem::remove_all(test_dir);
        std::filesystem::create_directories(test_dir);
    }

    void TearDown() override { std::filesystem::remove_all(test_dir); }

    std::unordered_map<CostCategory, std::pair<double, double>>
    BuildCosts() const {
        std::unordered_map<CostCategory, std::pair<double, double>> costs;
        for (int i = 0; i < static_cast<int>(CostCategory::kCount); ++i) {
            costs[static_cast<CostCategory>(i)] = {
                static_cast<double>(i), static_cast<double>(i) + 0.5};
        }
        return costs;
    }

    std::unique_ptr<Person> BuildPersonWithRowAndCosts(
        const std::string &row,
        const std::unordered_map<CostCategory, std::pair<double, double>>
            &costs) {
        auto person = std::make_unique<NiceMock<MockPerson>>();
        ON_CALL(*person, MakePopulationRow()).WillByDefault(Return(row));
        ON_CALL(*person, GetCosts()).WillByDefault(Return(costs));
        return person;
    }

    std::vector<std::string> ReadLines(const std::filesystem::path &path) {
        std::ifstream in(path);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(in, line)) {
            lines.push_back(line);
        }
        return lines;
    }
};

TEST_F(DataWriterTest, CreateMakesOutputDirectoryWhenMissing) {
    std::filesystem::path output_dir = test_dir / "new_output_dir";
    EXPECT_FALSE(std::filesystem::exists(output_dir));

    auto writer = Writer::Create(output_dir.string(), "WriterTest");

    EXPECT_NE(writer, nullptr);
    EXPECT_TRUE(std::filesystem::exists(output_dir));
}

TEST_F(DataWriterTest, WritePopulationUsesDefaultSequentialIdsWhenNotProvided) {
    auto writer = Writer::Create((test_dir / "out").string(), "WriterTest");
    auto costs = BuildCosts();

    People population;
    population.push_back(BuildPersonWithRowAndCosts("row-one", costs));
    population.push_back(BuildPersonWithRowAndCosts("row-two", costs));

    std::filesystem::path out_file = test_dir / "population_default_ids.csv";
    auto status = writer->WritePopulation(population, out_file.string(),
                                          OutputType::kFile);

    EXPECT_EQ(status, "success");
    auto lines = ReadLines(out_file);
    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[1], "1,row-one");
    EXPECT_EQ(lines[2], "2,row-two");
}

TEST_F(DataWriterTest, WritePopulationUsesProvidedIdsWhenGiven) {
    auto writer = Writer::Create((test_dir / "out").string(), "WriterTest");
    auto costs = BuildCosts();

    People population;
    population.push_back(BuildPersonWithRowAndCosts("alpha", costs));
    population.push_back(BuildPersonWithRowAndCosts("beta", costs));

    std::filesystem::path out_file = test_dir / "population_custom_ids.csv";
    auto status = writer->WritePopulation(population, out_file.string(),
                                          OutputType::kFile, {8, 9});

    EXPECT_EQ(status, "success");
    auto lines = ReadLines(out_file);
    ASSERT_EQ(lines.size(), 3);
    EXPECT_EQ(lines[1], "8,alpha");
    EXPECT_EQ(lines[2], "9,beta");
}

TEST_F(DataWriterTest,
       WriteCostsByCategoryWritesAllCategoriesWithoutTrailingComma) {
    auto writer = Writer::Create((test_dir / "out").string(), "WriterTest");
    auto costs = BuildCosts();

    People population;
    population.push_back(BuildPersonWithRowAndCosts("unused", costs));

    std::filesystem::path out_file = test_dir / "costs.csv";
    auto status = writer->WriteCostsByCategory(population, out_file.string(),
                                               OutputType::kFile, {42});

    EXPECT_EQ(status, "success");
    auto lines = ReadLines(out_file);
    ASSERT_EQ(lines.size(), 2);

    EXPECT_EQ(lines[1].rfind("42,", 0), 0u);
    EXPECT_NE(lines[1].find(",10,10.5"), std::string::npos);
    ASSERT_FALSE(lines[1].empty());
    EXPECT_NE(lines[1].back(), ',');
}

TEST_F(DataWriterTest,
       WritePopulationReturnsEmptyStringWhenFileCannotBeOpened) {
    auto writer = Writer::Create((test_dir / "out").string(), "WriterTest");
    auto costs = BuildCosts();

    People population;
    population.push_back(BuildPersonWithRowAndCosts("row", costs));

    std::filesystem::path invalid_path =
        test_dir / "missing_parent" / "population.csv";
    auto status = writer->WritePopulation(population, invalid_path.string(),
                                          OutputType::kFile);

    EXPECT_EQ(status, "");
}

} // namespace testing
} // namespace hepce

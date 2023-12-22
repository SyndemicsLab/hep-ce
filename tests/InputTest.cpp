#include "Inputs.hpp"
#include <boost/filesystem.hpp>
#include <gtest/gtest.h>

class InputTest : public ::testing::Test {
protected:
    std::ofstream outStream;
    boost::filesystem::path tempFilePath =
        boost::filesystem::temp_directory_path();

    void SetUp() override {
        tempFilePath /= boost::filesystem::unique_path("%%%%.conf");
        outStream.open(tempFilePath);
    }

    void TearDown() override {
        if (outStream.is_open()) {
            outStream.close();
        }
    }
};

TEST_F(InputTest, GetSeed) {
    outStream << "[simulation]\nseed = 12" << std::endl;
    Data::Config config(tempFilePath.string());
    EXPECT_EQ(12, config.getSeed());
}

TEST_F(InputTest, NoSeed) {
    outStream << "[simulation]\nseed =" << std::endl;
    Data::Config config(tempFilePath.string());
    EXPECT_EQ(std::nullopt, config.getSeed());
}

TEST_F(InputTest, IneligibleDrugUse) {
    outStream << "[eligibility]\nineligible_drug_use = NONINJECTION, INJECTION"
              << std::endl;
    Data::Config config(tempFilePath.string());
    std::vector<std::string> ineligibleDrugUse =
        config.getIneligibleDrugUse().value();
    std::vector<std::string> expected = {"NONINJECTION", "INJECTION"};
    EXPECT_EQ(expected, ineligibleDrugUse);
}

TEST_F(InputTest, ScreeningTest) {
    outStream << "[screening_background_ab]" << std::endl
              << "cost = 100.00" << std::endl
              << "acute_sensitivity = 0.97" << std::endl
              << "chronic_sensitivity = 0.97" << std::endl
              << "specificity = 0.97" << std::endl;
    Data::Config config(tempFilePath.string());
    std::vector<double> screenTest = config.getScreenTest("background_ab");
    std::vector<double> expected = {100.00, 0.97, 0.97, 0.97};
    EXPECT_EQ(expected, screenTest);
}

TEST_F(InputTest, GetLinkingInterventionCost) {
    outStream << "[linking]\nintervention_cost = 100.00" << std::endl;
    Data::Config config(tempFilePath.string());
    EXPECT_EQ(100.00, config.getLinkingInterventionCost());
}

#include "Cost.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <numeric>

TEST(CostTest, AddCost) {
    cost::CostTracker ct;
    // add cost with default values
    cost::Cost toAdd;
    ct.addCost(toAdd, 0);
    cost::Cost test = ct.getCosts()[0][0];
    EXPECT_EQ(test.category, cost::CostCategory::MISC);
    EXPECT_EQ(test.name, "");
    EXPECT_EQ(test.cost, 0.00);
    // add cost with specific values
    toAdd.category = cost::CostCategory::LINKING;
    toAdd.name = "test";
    toAdd.cost = 100.00;
    ct.addCost(toAdd, 0);
    test = ct.getCosts()[0][1];
    EXPECT_EQ(test.category, cost::CostCategory::LINKING);
    EXPECT_EQ(test.name, "test");
    EXPECT_EQ(test.cost, 100.00);
}

TEST(CostTest, DefaultConstructorGetTotals) {
    cost::CostTracker ct;
    std::unordered_map<int, double> expected = {};
    EXPECT_EQ(expected, ct.getTotals());
}

TEST(CostTest, GetCostsAndTotals) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    // populate costtracker
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        cost::Cost toAdd = {cost::CostCategory::BEHAVIOR, "test", (double)i};
        ct.addCost(toAdd, i);
    }

    // populate expected cost object
    std::unordered_map<int, std::vector<cost::Cost>> expectedCosts = {};
    cost::CostCategory expectedCategory = cost::CostCategory::BEHAVIOR;
    std::string expectedName = "test";
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        expectedCosts[i].push_back(
            {(cost::Cost){expectedCategory, expectedName, (double)i}});
    }

    // compare generated to expected
    std::unordered_map<int, std::vector<cost::Cost>> addedCosts = ct.getCosts();
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        EXPECT_EQ(expectedCosts[i][0].category, addedCosts[i][0].category);
        EXPECT_EQ(expectedCosts[i][0].name, addedCosts[i][0].name);
        EXPECT_EQ(expectedCosts[i][0].cost, addedCosts[i][0].cost);
    }

    std::unordered_map<int, double> expectedTotals;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        expectedTotals[i] = (double)i;
    }
    EXPECT_EQ(expectedTotals, ct.getTotals());

    std::unordered_map<int, double> expectedDiscountedTotals;
    double discountRate = 0.03;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        expectedDiscountedTotals[i] =
            expectedTotals[i] / std::pow(1 + discountRate / 12, i + 1);
    }
    EXPECT_EQ(expectedDiscountedTotals, ct.getTotals(discountRate));
}

TEST(CostTest, GetTotalsByCategory) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    std::string name = "test";

    // populate all categories
    for (int i = 0; i < (int)cost::CostCategory::COUNT; ++i) {
        // iterate over time horizon
        for (int timestep = 0; timestep < ELEMENT_COUNT; ++timestep) {
            cost::Cost cost = {(cost::CostCategory)i, name, (double)timestep};
            ct.addCost(cost, timestep);
        }
    }

    std::vector<double> expectedTotals(ELEMENT_COUNT);
    std::iota(expectedTotals.begin(), expectedTotals.end(), 0);
    std::unordered_map<cost::CostCategory, std::vector<double>>
        totalsByCategory = ct.getTotalsByCategory();
    for (int i = 0; i < (int)cost::CostCategory::COUNT; ++i) {
        for (int timestep = 0; timestep < ELEMENT_COUNT; ++timestep) {
            EXPECT_EQ(expectedTotals[timestep],
                      totalsByCategory[(cost::CostCategory)i][timestep]);
        }
    }

    std::vector<double> expectedDiscountedTotals(ELEMENT_COUNT);
    double discountRate = 0.03;
    totalsByCategory = ct.getTotalsByCategory(discountRate);

    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        expectedDiscountedTotals[i] =
            expectedTotals[i] / std::pow(1 + discountRate / 12, i + 1);
    }
    for (int i = 0; i < (int)cost::CostCategory::COUNT; ++i) {
        for (int timestep = 0; timestep < ELEMENT_COUNT; ++timestep) {
            EXPECT_EQ(expectedDiscountedTotals[timestep],
                      totalsByCategory[(cost::CostCategory)i][timestep]);
        }
    }
}

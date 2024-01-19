#include "Cost.hpp"
#include <gtest/gtest.h>
#include <numeric>

TEST(CostTest, AddCost) {
    Cost::CostTracker ct;
    // add cost with default values
    Cost::Cost toAdd;
    ct.addCost(toAdd, 0);
    Cost::Cost test = ct.getCosts()[0][0];
    EXPECT_EQ(test.category, Cost::CostCategory::MISC);
    EXPECT_EQ(test.name, "");
    EXPECT_EQ(test.cost, 0.00);
    // add cost with specific values
    toAdd.category = Cost::CostCategory::LINKING;
    toAdd.name = "test";
    toAdd.cost = 100.00;
    ct.addCost(toAdd, 0);
    test = ct.getCosts()[0][1];
    EXPECT_EQ(test.category, Cost::CostCategory::LINKING);
    EXPECT_EQ(test.name, "test");
    EXPECT_EQ(test.cost, 100.00);
}

TEST(CostTest, GetCostsAndTotals) {
    int ELEMENT_COUNT = 10;
    Cost::CostTracker ct(ELEMENT_COUNT);
    std::vector<std::vector<Cost::Cost>> expectedCosts = {};
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        Cost::Cost toAdd = {Cost::CostCategory::BEHAVIOR, "test", (double)i};
        ct.addCost(toAdd, i);
        expectedCosts.push_back({toAdd});
    }
    const std::vector<std::vector<Cost::Cost>> &addedCosts = ct.getCosts();
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        EXPECT_EQ(expectedCosts[i][0].category, addedCosts[i][0].category);
        EXPECT_EQ(expectedCosts[i][0].name, addedCosts[i][0].name);
        EXPECT_EQ(expectedCosts[i][0].cost, addedCosts[i][0].cost);
    }

    std::vector<double> expectedTotals(ELEMENT_COUNT);
    std::iota(expectedTotals.begin(), expectedTotals.end(), 0);
    EXPECT_EQ(expectedTotals, ct.getTotals());
}

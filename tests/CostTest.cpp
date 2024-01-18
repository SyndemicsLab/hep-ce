#include "Cost.hpp"
#include <gtest/gtest.h>

TEST(CostTest, AddCost) {
    Cost::CostTracker ct;
    Cost::Cost toAdd = {"EVENT.category", 100.00};
    ct.addCost(toAdd, 0);
    std::vector<Cost::Cost> expected = {toAdd};
    EXPECT_EQ(expected[0].category, "EVENT.category");
    EXPECT_EQ(expected[0].cost, 100.00);
}

TEST(CostTest, GetCostsAndTotals) {
    Cost::CostTracker ct(10);
    std::vector<std::vector<Cost::Cost>> expectedCosts = {};
    for (int i = 0; i < 10; ++i) {
        Cost::Cost toAdd = {"EVENT.category", (double)i};
        ct.addCost(toAdd, i);
        expectedCosts.push_back({toAdd});
    }
    const std::vector<std::vector<Cost::Cost>> &addedCosts = ct.getCosts();
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(expectedCosts[i][0].category, addedCosts[i][0].category);
        EXPECT_EQ(expectedCosts[i][0].cost, addedCosts[i][0].cost);
    }

    std::vector<double> expectedTotals = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(expectedTotals, ct.getTotals());
}

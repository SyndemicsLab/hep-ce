#include "Cost.hpp"

TEST(CostTest, AddCost) {
    Cost::CostTracker ct;
    Cost::Cost toAdd = {"EVENT.category", 100.00};
    ct.addCost(toAdd, 0);
    std::vector<Cost::Cost> expected = {toAdd};
    EXPECT_EQ(expected, ct.getCosts());
}

TEST(CostTest, GetCostsAndTotals) {
    Cost::CostTracker ct;
    std::vector<std::vector<Cost::Cost>> expectedCosts = {};
    for (int i = 0; i < 10; ++i) {
        Cost::Cost toAdd = {"EVENT.category", (double)i};
        ct.addCost(toAdd, i);
        expectedCosts.push_back({toAdd});
    }
    EXPECT_EQ(expectedCosts, ct.getCosts());

    std::vector<double> expectedTotals = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    EXPECT_EQ(expectedTotals, ct.getTotals());
}

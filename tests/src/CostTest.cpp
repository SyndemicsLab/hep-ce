#include "Cost.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <numeric>

TEST(CostTest, AddCost) {
    cost::CostTracker ct;

    double cost = 25.25;
    ct.AddCost(cost, cost::CostCategory::MISC);
    auto test = ct.GetCosts();
    EXPECT_EQ(test[cost::CostCategory::MISC], cost);
}

TEST(CostTest, GetTotalSingleCostCategory) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    // populate costtracker
    double sum = 0.0;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        ct.AddCost((double)i, cost::CostCategory::BEHAVIOR);
        sum += (double)i;
    }
    EXPECT_EQ(ct.GetTotal(), sum);
}

TEST(CostTest, GetTotalMultiCostCategory) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    // populate costtracker
    double sum = 0.0;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        if (i % 2 == 0) {
            ct.AddCost((double)i, cost::CostCategory::MISC);
        } else {
            ct.AddCost((double)i, cost::CostCategory::BEHAVIOR);
        }

        sum += (double)i;
    }
    EXPECT_EQ(ct.GetTotal(), sum);
}

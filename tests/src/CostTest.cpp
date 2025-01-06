#include "Cost.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <numeric>

TEST(CostTest, AddCost) {
    cost::CostTracker ct;

    double base_cost = 25.25;
    double discount_cost = 15.00;
    ct.AddCost(base_cost, discount_cost, cost::CostCategory::MISC);
    auto test = ct.GetCosts();
    EXPECT_EQ(test[cost::CostCategory::MISC].first, base_cost);
    EXPECT_EQ(test[cost::CostCategory::MISC].second, discount_cost);
}

TEST(CostTest, GetTotalSingleCostCategory) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    // populate costtracker
    double base_sum = 0.0;
    double discount_sum = 0.0;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        ct.AddCost((double)i, ((double)i / 2), cost::CostCategory::BEHAVIOR);
        base_sum += (double)i;
        discount_sum += ((double)i / 2);
    }
    EXPECT_EQ(ct.GetTotals().first, base_sum);
    EXPECT_EQ(ct.GetTotals().second, discount_sum);
}

TEST(CostTest, GetTotalMultiCostCategory) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    // populate costtracker
    double sum = 0.0;
    double discount_sum = 0.0;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        if (i % 2 == 0) {
            ct.AddCost((double)i, ((double)i / 2), cost::CostCategory::MISC);
        } else {
            ct.AddCost((double)i, ((double)i / 2),
                       cost::CostCategory::BEHAVIOR);
        }

        sum += (double)i;
        discount_sum += ((double)i / 2);
    }
    EXPECT_EQ(ct.GetTotals().first, sum);
    EXPECT_EQ(ct.GetTotals().second, discount_sum);
}

TEST(CostTest, GetCostCategory) {
    int ELEMENT_COUNT = 10;
    cost::CostTracker ct;
    // populate costtracker
    double misc_sum = 0.0;
    double misc_d_sum = 0.0;
    double behavior_sum = 0.0;
    double behavior_d_sum = 0.0;
    for (int i = 0; i < ELEMENT_COUNT; ++i) {
        if (i % 2 == 0) {
            ct.AddCost((double)i, ((double)i / 2), cost::CostCategory::MISC);
            misc_sum += (double)i;
            misc_d_sum += ((double)i / 2);
        } else {
            ct.AddCost((double)i, ((double)i / 2),
                       cost::CostCategory::BEHAVIOR);
            behavior_sum += (double)i;
            behavior_d_sum += ((double)i / 2);
        }
    }
    EXPECT_EQ(ct.GetCosts()[cost::CostCategory::MISC].first, misc_sum);
    EXPECT_EQ(ct.GetCosts()[cost::CostCategory::MISC].second, misc_d_sum);
    EXPECT_EQ(ct.GetCosts()[cost::CostCategory::BEHAVIOR].first, behavior_sum);
    EXPECT_EQ(ct.GetCosts()[cost::CostCategory::BEHAVIOR].second,
              behavior_d_sum);
}

////////////////////////////////////////////////////////////////////////////////
// File: costing_test.cpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2026-04-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/costing.hpp>

#include <sstream>

#include <gtest/gtest.h>

namespace hepce {
namespace testing {

using namespace hepce::model;

TEST(CostingTest, CreateInitializesAllCategoriesToZero) {
    auto costs = Costs::Create();
    const auto all_costs = costs->GetCosts();

    ASSERT_EQ(all_costs.size(), static_cast<size_t>(CostCategory::kCount));
    for (int i = 0; i < static_cast<int>(CostCategory::kCount); ++i) {
        const auto category = static_cast<CostCategory>(i);
        ASSERT_TRUE(all_costs.find(category) != all_costs.end());
        EXPECT_DOUBLE_EQ(all_costs.at(category).first, 0.0);
        EXPECT_DOUBLE_EQ(all_costs.at(category).second, 0.0);
    }
}

TEST(CostingTest, AddCostAccumulatesAndTotalsAcrossCategories) {
    auto costs = Costs::Create();

    costs->AddCost(10.0, 7.0, CostCategory::kBehavior);
    costs->AddCost(5.0, 2.5, CostCategory::kBehavior);
    costs->AddCost(3.0, 1.0, CostCategory::kTreatment);

    const auto by_category = costs->GetCosts();
    EXPECT_DOUBLE_EQ(by_category.at(CostCategory::kBehavior).first, 15.0);
    EXPECT_DOUBLE_EQ(by_category.at(CostCategory::kBehavior).second, 9.5);
    EXPECT_DOUBLE_EQ(by_category.at(CostCategory::kTreatment).first, 3.0);
    EXPECT_DOUBLE_EQ(by_category.at(CostCategory::kTreatment).second, 1.0);

    const auto totals = costs->GetTotals();
    EXPECT_DOUBLE_EQ(totals.first, 18.0);
    EXPECT_DOUBLE_EQ(totals.second, 10.5);
}

TEST(CostingTest, CloneCreatesIndependentZeroedTracker) {
    auto costs = Costs::Create();
    costs->AddCost(4.0, 3.0, CostCategory::kMisc);

    auto clone = costs->clone();
    ASSERT_NE(clone, nullptr);

    const auto cloned_totals = clone->GetTotals();
    EXPECT_DOUBLE_EQ(cloned_totals.first, 0.0);
    EXPECT_DOUBLE_EQ(cloned_totals.second, 0.0);

    const auto original_totals = costs->GetTotals();
    EXPECT_DOUBLE_EQ(original_totals.first, 4.0);
    EXPECT_DOUBLE_EQ(original_totals.second, 3.0);
}

TEST(CostingTest, CostCategoryStreamOutputHandlesKnownAndUnknownValues) {
    std::stringstream ss;
    ss << CostCategory::kOverdose << " " << static_cast<CostCategory>(999);

    EXPECT_EQ(ss.str(), "kOverdose na");
}

TEST(CostingTest, CostCategoryStreamOutputCoversAllKnownCategories) {
    std::stringstream ss;
    ss << CostCategory::kMisc << "," << CostCategory::kBehavior << ","
       << CostCategory::kScreening << "," << CostCategory::kLinking << ","
       << CostCategory::kStaging << "," << CostCategory::kLiver << ","
       << CostCategory::kTreatment << "," << CostCategory::kBackground << ","
       << CostCategory::kMoud << "," << CostCategory::kOverdose;

    EXPECT_EQ(ss.str(), "kMisc,kBehavior,kScreening,kLinking,kStaging,kLiver,"
                        "kTreatment,kBackground,kMoud,kOverdose");
}

} // namespace testing
} // namespace hepce

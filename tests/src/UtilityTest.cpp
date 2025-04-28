////////////////////////////////////////////////////////////////////////////////
// File: UtilityTest.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-02-27                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Utility.hpp"

#include <gtest/gtest.h>

TEST(UtilityTest, SetUtility) {
    utility::UtilityTracker ut;

    double util = 0.8;
    ut.SetUtility(util, utility::UtilityCategory::BEHAVIOR);
    auto test = ut.GetRawUtilities();
    EXPECT_EQ(test.at(utility::UtilityCategory::BEHAVIOR), 0.8);
}

TEST(UtilityTest, GetUtilities) {
    utility::UtilityTracker ut;

    double expected_min = 0.64;
    double expected_mult = 1.0;
    for (int i = 0; i < (int)utility::UtilityCategory::kCount; ++i) {
        double util = 1 - 0.36 / (static_cast<int>(i) + 1);
        expected_mult *= util;
        utility::UtilityCategory category =
            static_cast<utility::UtilityCategory>(i);
        ut.SetUtility(util, category);
    }

    std::pair<double, double> expected = {expected_min, expected_mult};

    EXPECT_EQ(ut.GetUtilities(), expected);
}

TEST(UtilityTest, GetRawUtilities) {
    utility::UtilityTracker ut;
    std::unordered_map<utility::UtilityCategory, double> expected;

    for (int i = 0; i < (int)utility::UtilityCategory::kCount; ++i) {
        double util = 1 - 0.36 / (static_cast<int>(i) + 1);
        utility::UtilityCategory category =
            static_cast<utility::UtilityCategory>(i);
        expected[category] = util;
        ut.SetUtility(util, category);
    }

    EXPECT_EQ(ut.GetRawUtilities(), expected);
}

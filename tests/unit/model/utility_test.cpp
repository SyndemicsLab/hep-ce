////////////////////////////////////////////////////////////////////////////////
// File: utility_test.cpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2026-04-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/utility.hpp>

#include <sstream>

#include <gtest/gtest.h>

namespace hepce {
namespace testing {

using namespace hepce::model;

TEST(UtilityTest, StreamOutputForAllKnownCategories) {
    std::stringstream ss;
    ss << UtilityCategory::kBehavior << " " << UtilityCategory::kLiver << " "
       << UtilityCategory::kTreatment << " " << UtilityCategory::kBackground
       << " " << UtilityCategory::kHiv << " " << UtilityCategory::kMoud << " "
       << UtilityCategory::kOverdose;

    EXPECT_EQ(ss.str(),
              "kBehavior kLiver kTreatment kBackground kHiv kMoud kOverdose");
}

TEST(UtilityTest, StreamOutputFallsBackToNaForUnknownCategory) {
    std::stringstream ss;
    ss << static_cast<UtilityCategory>(999);

    EXPECT_EQ(ss.str(), "kNa");
}

} // namespace testing
} // namespace hepce

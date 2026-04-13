////////////////////////////////////////////////////////////////////////////////
// File: sampler_test.cpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2026-04-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/sampler.hpp>

#include <gtest/gtest.h>

namespace hepce {
namespace testing {

TEST(SamplerTest, ReturnsNegativeOneWhenProbabilitiesExceedOne) {
    auto sampler = model::Sampler::Create(1234, "SamplerTest");
    const std::vector<double> probabilities = {0.6, 0.5};

    EXPECT_EQ(sampler->GetDecision(probabilities), -1);
}

TEST(SamplerTest, ReturnsZeroWhenSingleBucketHasFullProbability) {
    auto sampler = model::Sampler::Create(42, "SamplerTest");
    const std::vector<double> probabilities = {1.0};

    EXPECT_EQ(sampler->GetDecision(probabilities), 0);
}

TEST(SamplerTest, ReturnsSizeWhenDistributionNeverCrossesReference) {
    auto sampler = model::Sampler::Create(42, "SamplerTest");
    const std::vector<double> probabilities = {0.0};

    EXPECT_EQ(sampler->GetDecision(probabilities), 1);
}

TEST(SamplerTest, ClonePreservesRandomGeneratorState) {
    auto sampler = model::Sampler::Create(2026, "SamplerCloneTest");
    const std::vector<double> probabilities = {0.3, 0.7};

    // Advance generator state before cloning.
    (void)sampler->GetDecision(probabilities);
    auto cloned_sampler = sampler->clone();

    EXPECT_EQ(sampler->GetDecision(probabilities),
              cloned_sampler->GetDecision(probabilities));
    EXPECT_EQ(sampler->GetDecision(probabilities),
              cloned_sampler->GetDecision(probabilities));
}

} // namespace testing
} // namespace hepce

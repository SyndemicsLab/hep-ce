////////////////////////////////////////////////////////////////////////////////
// File: sampler_mock.hpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef HEPCE_TESTS_SAMPLERMOCK_HPP_
#define HEPCE_TESTS_SAMPLERMOCK_HPP_

#include <hepce/model/sampler.hpp>

#include <gmock/gmock.h>

using namespace hepce::model;

namespace hepce {
namespace testing {
class MockSampler : public virtual Sampler {
public:
    MOCK_METHOD(const int, GetDecision, (const std::vector<double> &probs),
                (override));
};
} // namespace testing
} // namespace hepce

#endif

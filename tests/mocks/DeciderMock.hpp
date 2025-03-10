////////////////////////////////////////////////////////////////////////////////
// File: DeciderMock.hpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DECIDERMOCK_HPP_
#define DECIDERMOCK_HPP_

#include "Decider.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace stats {
class MOCKDecider : public DeciderBase {
public:
    MOCK_METHOD(int, LoadGenerator, (std::mt19937_64 const generator),
                (override));
    MOCK_METHOD(int, GetDecision, (std::vector<double> probs), (override));
};
} // namespace stats

#endif

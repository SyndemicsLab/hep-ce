////////////////////////////////////////////////////////////////////////////////
// File: costing_mock.hpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2025-05-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_TESTS_COSTINGMOCK_HPP_
#define HEPCE_TESTS_COSTINGMOCK_HPP_

#include <hepce/model/costing.hpp>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gmock/gmock.h>

using namespace hepce::model;

namespace hepce {
namespace testing {
class MockCosts : public virtual Costs {
public:
    MOCK_METHOD((std::pair<double, double>), GetTotals, (), (const, override));
    MOCK_METHOD((std::unordered_map<CostCategory, std::pair<double, double>>),
                GetCosts, (), (const, override));
    MOCK_METHOD(void, AddCost, (double, double, CostCategory), (override));
};
} // namespace testing
} // namespace hepce

#endif

////////////////////////////////////////////////////////////////////////////////
// File: costing.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-22                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/costing.hpp>

#include <memory>

#include "internals/costing_internals.hpp"

namespace hepce {
namespace model {
std::unique_ptr<Costs> Costs::Create(const std::string &log_name) {
    return std::make_unique<CostsImpl>(log_name);
}

CostsImpl::CostsImpl(const std::string &log_name) {
    for (int i = 0; i != static_cast<int>(CostCategory::kCount); ++i) {
        _costs[static_cast<CostCategory>(i)] = {0.0, 0.0};
    }
}

std::pair<double, double> CostsImpl::GetTotals() const {
    double base_sum = 0.0;
    double discount_sum = 0.0;
    for (auto const &kv : _costs) {
        base_sum += kv.second.first;
        discount_sum += kv.second.second;
    }
    return {base_sum, discount_sum};
}

std::ostream &operator<<(std::ostream &os, const CostCategory &inst) {
    switch (inst) {
    case CostCategory::kMisc:
        os << "kMisc";
        break;
    case CostCategory::kBehavior:
        os << "kBehavior";
        break;
    case CostCategory::kScreening:
        os << "kScreening";
        break;
    case CostCategory::kLinking:
        os << "kLinking";
        break;
    case CostCategory::kStaging:
        os << "kStaging";
        break;
    case CostCategory::kLiver:
        os << "kLiver";
        break;
    case CostCategory::kTreatment:
        os << "kTreatment";
        break;
    case CostCategory::kBackground:
        os << "kBackground";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
} // namespace model
} // namespace hepce
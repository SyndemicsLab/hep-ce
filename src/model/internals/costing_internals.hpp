////////////////////////////////////////////////////////////////////////////////
// File: cost_internals.hpp                                                   //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_COSTINTERNALS_HPP_
#define HEPCE_MODEL_COSTINTERNALS_HPP_

#include <hepce/model/costing.hpp>

namespace hepce {
namespace model {
class CostsImpl : public virtual Costs {
public:
    CostsImpl(const std::string &log_name);
    ~CostsImpl() = default;

    std::pair<double, double> GetTotals() const override;

    inline std::unordered_map<CostCategory, std::pair<double, double>>
    GetCosts() const override {
        return _costs;
    }

    inline void AddCost(double base_cost, double discount_cost,
                        CostCategory category) override {
        _costs[category].first += base_cost;
        _costs[category].second += discount_cost;
    }

private:
    std::unordered_map<CostCategory, std::pair<double, double>> _costs;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_COSTINTERNALS_HPP_
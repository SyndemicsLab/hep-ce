////////////////////////////////////////////////////////////////////////////////
// File: costing.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_COST_HPP_
#define HEPCE_MODEL_COST_HPP_

#include <memory>
#include <ostream>
#include <unordered_map>
#include <utility>

namespace hepce {
namespace model {
enum class CostCategory : int {
    kMisc = 0,
    kBehavior = 1,
    kScreening = 2,
    kLinking = 3,
    kStaging = 4,
    kLiver = 5,
    kTreatment = 6,
    kBackground = 7,
    kHiv = 8,
    kCount = 9
};

std::ostream &operator<<(std::ostream &os, const CostCategory &inst);

class Costs {
public:
    virtual ~Costs() = default;
    virtual std::pair<double, double> GetTotals() const = 0;

    /// @brief Get the vector of costs as-is
    /// @return Vector of vectors of \code{Cost} objects, each element
    /// of the top-level vector representing the timestep during which
    /// the cost was accrued
    virtual std::unordered_map<CostCategory, std::pair<double, double>>
    GetCosts() const = 0;

    /// @brief Add a cost to the tracker
    /// @param cost The \code{Cost} item to be added
    /// @param timestep The timestep during which this cost is added
    virtual void AddCost(double base_cost, double discount_cost,
                         CostCategory category) = 0;

    static std::unique_ptr<Costs>
    Create(const std::string &log_name = "console");
};

} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_COST_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: Cost.hpp                                                             //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-04-10                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef COST_COST_HPP_
#define COST_COST_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cost {
/// @brief Categories for classifying cost items
enum class CostCategory {
    MISC = 0,
    BEHAVIOR = 1,
    SCREENING = 2,
    LINKING = 3,
    STAGING = 4,
    LIVER = 5,
    TREATMENT = 6,
    BACKGROUND = 7,
    COUNT = 8
};
std::ostream &operator<<(std::ostream &os, const CostCategory &inst);

class CostTrackerBase {
public:
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
};

class CostTracker : public CostTrackerBase {
private:
    class CostTrackerIMPL;
    std::unique_ptr<CostTrackerIMPL> impl;

public:
    CostTracker();
    ~CostTracker();

    // Copy Operations
    CostTracker(CostTracker const &) = delete;
    CostTracker &operator=(CostTracker const &) = delete;
    CostTracker(CostTracker &&) noexcept;
    CostTracker &operator=(CostTracker &&) noexcept;

    std::pair<double, double> GetTotals() const override;

    /// @brief Get the vector of costs as-is
    /// @return Vector of vectors of \code{Cost} objects, each element
    /// of the top-level vector representing the timestep during which
    /// the cost was accrued
    std::unordered_map<CostCategory, std::pair<double, double>>
    GetCosts() const override;

    /// @brief Add a cost to the tracker
    /// @param cost The \code{Cost} item to be added
    /// @param timestep The timestep during which this cost is added
    void AddCost(double base_cost, double discount_cost,
                 CostCategory category = CostCategory::MISC) override;
};
} // namespace cost
#endif

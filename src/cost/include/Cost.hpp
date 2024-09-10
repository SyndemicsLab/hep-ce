//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of objects used for costs tracking in
/// the simulation.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef COST_COST_HPP_
#define COST_COST_HPP_

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

namespace Cost {
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

    /// @brief Object to hold individual charges/costs
    struct Cost {
        CostCategory category = CostCategory::MISC;
        std::string name = "";
        double cost = 0;
    };
    std::ostream &operator<<(std::ostream &os, const Cost &cost);

    class CostTracker {
    public:
        CostTracker() {}

        /// @brief Get a vector of total costs per timestep
        /// @return Vector of total costs per timestep
        std::unordered_map<int, double> getTotals() const;

        /// @brief Get a vector of discounted total costs per timestep
        /// @param discountRate The discount rate
        /// @return Vector of total discounted costs per timestep
        std::unordered_map<int, double> getTotals(double discountRate) const;

        /// @brief Get the vector of costs as-is
        /// @return Vector of vectors of \code{Cost} objects, each element
        /// of the top-level vector representing the timestep during which
        /// the cost was accrued
        std::unordered_map<int, std::vector<Cost>> getCosts() const {
            return this->costs;
        }

        /// @brief Get category-stratified costs over the time horizon
        /// @return Category-stratified costs
        std::unordered_map<CostCategory, std::vector<double>>
        getTotalsByCategory() const;

        /// @brief Get discounted category-stratified costs over the time
        /// horizon
        /// @return Discounted, category-stratified costs
        std::unordered_map<CostCategory, std::vector<double>>
        getTotalsByCategory(double discountRate) const;

        /// @brief Add a cost to the tracker
        /// @param cost The \code{Cost} item to be added
        /// @param timestep The timestep during which this cost is added
        void addCost(Cost cost, int timestep);

    private:
        std::unordered_map<int, std::vector<Cost>> costs = {};
    };
} // namespace Cost
#endif

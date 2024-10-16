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

#include <memory>
#include <string>
#include <unordered_map>
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
        /// @brief Get a vector of discounted total costs per timestep
        /// @param discountRate The discount rate
        /// @return Vector of total discounted costs per timestep
        virtual double GetTotal() const = 0;

        /// @brief Get the vector of costs as-is
        /// @return Vector of vectors of \code{Cost} objects, each element
        /// of the top-level vector representing the timestep during which
        /// the cost was accrued
        virtual std::unordered_map<CostCategory, double> GetCosts() const = 0;

        /// @brief Add a cost to the tracker
        /// @param cost The \code{Cost} item to be added
        /// @param timestep The timestep during which this cost is added
        virtual void AddCost(double cost, CostCategory category) = 0;
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
        /// @brief Get a vector of discounted total costs per timestep
        /// @param discountRate The discount rate
        /// @return Vector of total discounted costs per timestep
        double GetTotal() const override;

        /// @brief Get the vector of costs as-is
        /// @return Vector of vectors of \code{Cost} objects, each element
        /// of the top-level vector representing the timestep during which
        /// the cost was accrued
        std::unordered_map<CostCategory, double> GetCosts() const override;

        /// @brief Add a cost to the tracker
        /// @param cost The \code{Cost} item to be added
        /// @param timestep The timestep during which this cost is added
        void AddCost(double cost,
                     CostCategory category = CostCategory::MISC) override;
    };
} // namespace cost
#endif

#include "Cost.hpp"

namespace Cost {
    std::unordered_map<int, double> CostTracker::getTotals() const {
        std::unordered_map<int, double> totals = {};
        for (auto &timestep : this->costs) {
            totals[timestep.first] = 0;
            for (const Cost &cost : timestep.second) {
                totals[timestep.first] += cost.cost;
            }
        }
        return totals;
    }

    std::unordered_map<int, double>
    CostTracker::getTotals(double discountRate) const {
        std::unordered_map<int, double> totals = {};

        for (auto &timestep : this->costs) {
            totals[timestep.first] = 0;
            for (const Cost &cost : timestep.second) {
                totals[timestep.first] += cost.cost;
            }
            // dividing discountRate by 12 because discount rates are annual
            double denominator =
                std::pow(1 + discountRate / 12, timestep.first + 1);
            totals[timestep.first] /= denominator;
        }
        return totals;
    }

    std::unordered_map<CostCategory, std::vector<double>>
    CostTracker::getTotalsByCategory() const {
        std::unordered_map<CostCategory, std::vector<double>> costsByCategory;

        for (int i = 0; i < (int)CostCategory::COUNT; ++i) {
            // this->costs.size() is the total number of timesteps
            costsByCategory[(CostCategory)i] =
                std::vector<double>(this->costs.size(), 0);
        }

        for (auto &timestep : this->costs) {
            for (const Cost &cost : timestep.second) {
                costsByCategory[cost.category][timestep.first] += cost.cost;
            }
        }
        return costsByCategory;
    }

    std::unordered_map<CostCategory, std::vector<double>>
    CostTracker::getTotalsByCategory(double discountRate) const {
        std::unordered_map<CostCategory, std::vector<double>> costsByCategory;

        for (int i = 0; i < (int)CostCategory::COUNT; ++i) {
            // this->costs.size() is the total number of timesteps
            costsByCategory[(CostCategory)i] =
                std::vector<double>(this->costs.size(), 0);
        }

        // typeof(timestep) = std::pair<int, std::vector<Cost>>
        // timestep.first is the timestep in which costs were accrued
        for (auto &timestep : this->costs) {
            for (const Cost &cost : timestep.second) {
                costsByCategory[cost.category][timestep.first] += cost.cost;
            }
            // dividing discountRate by 12 because discount rates are annual
            double denominator =
                std::pow(1 + discountRate / 12, timestep.first + 1);
            for (int i = 0; i < (int)CostCategory::COUNT; ++i) {
                costsByCategory[(CostCategory)i][timestep.first] /= denominator;
            }
        }
        return costsByCategory;
    }

    void CostTracker::addCost(Cost cost, int timestep) {
        if (timestep < 0) {
            // log error
            return;
        }
        this->costs[timestep].push_back(cost);
    }
} // namespace Cost

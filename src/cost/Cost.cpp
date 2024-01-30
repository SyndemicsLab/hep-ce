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
    CostTracker::getDiscountedTotals(double discountRate) const {
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

    std::unordered_map<CostCategory, std::unordered_map<int, std::vector<Cost>>>
    CostTracker::getCostsByCategory() const {
        std::unordered_map<CostCategory,
                           std::unordered_map<int, std::vector<Cost>>>
            costsByCategory;

        for (auto &timestep : this->costs) {
            for (const Cost &cost : timestep.second) {
                costsByCategory[cost.category][timestep.first].push_back(cost);
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

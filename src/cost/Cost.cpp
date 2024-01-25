#include "Cost.hpp"

namespace Cost {
    CostTracker::CostTracker(int duration) {
        if (duration <= 0) {
            // log error
            return;
        }
        for (int i = 0; i < duration; ++i) {
            costs.push_back({});
        }
    }

    std::vector<double> CostTracker::getTotals() const {
        std::vector<double> totals = {};
        for (std::vector<Cost> timestep : this->costs) {
            double temp = 0;
            for (Cost &cost : timestep) {
                temp += cost.cost;
            }
            totals.push_back(temp);
        }
        return totals;
    }

    std::vector<double>
    CostTracker::getDiscountedTotals(double discountRate) const {
        std::vector<double> totals = {};

        for (int i = 1; i <= this->costs.size(); ++i) {
            double temp = 0;
            for (const Cost &cost : this->costs[i - 1]) {
                temp += cost.cost;
            }
            // dividing by 12 because discount rates are annual
            double denominator = std::pow(1 + discountRate / 12, i);
            temp /= denominator;
            totals.push_back(temp);
        }
        return totals;
    }

    std::unordered_map<CostCategory, std::vector<std::vector<Cost>>>
    CostTracker::getCostsByCategory() const {
        std::unordered_map<CostCategory, std::vector<std::vector<Cost>>>
            costsByCategory;
        for (int i = 0; i < (int)CostCategory::COUNT; ++i) {
            std::vector<std::vector<Cost>> temp =
                std::vector<std::vector<Cost>>(this->costs.size());
            costsByCategory[(CostCategory)i] = temp;
        }

        for (int i = 0; i < this->costs.size(); ++i) {
            for (const Cost &cost : this->costs[i]) {
                costsByCategory[cost.category][i].push_back(cost);
            }
        }
        return costsByCategory;
    }

    void CostTracker::addCost(Cost cost, int timestep) {
        if (timestep >= costs.size()) {
            // log error
            return;
        }
        this->costs[timestep].push_back(cost);
    }
} // namespace Cost

#include "Cost.hpp"
#include <iostream>

namespace Cost {
    CostTracker::CostTracker(int duration) {
        for (int i = 0; i < duration; ++i) {
            costs.push_back({});
        }
    }

    std::vector<double> CostTracker::getTotals() {
        std::vector<double> totals = {};
        for (std::vector<Cost> &timestep : this->costs) {
            double temp = 0;
            for (Cost &cost : timestep) {
                temp += cost.cost;
            }
            totals.push_back(temp);
        }
        return totals;
    }

    void CostTracker::addCost(Cost cost, int timestep) {
        this->costs[timestep].push_back(cost);
    }
} // namespace Cost

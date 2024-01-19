#include "Cost.hpp"
#include <iostream>

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

    void CostTracker::addCost(Cost cost, int timestep) {
        if (timestep > costs.size()) {
            // log error
            return;
        }
        this->costs[timestep].push_back(cost);
    }
} // namespace Cost

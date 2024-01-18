#include "Cost.hpp"

std::vector<double> CostTracker::getTotals() {
    std::vector<double> totals = {};
    for (std::vector<Cost> &timestep : this->costs) {
        double temp = 0;
        for (Cost &cost : timestep) {
            temp += cost.cost;
        }
        totals.push_back(temp);
    }
}

void CostTracker::addCost(Cost cost, int timestep) {
    // ensure the cost vector is sufficiently large
    if (this->costs.size() < (timestep + 1)) {
        this->costs.reserve(((size_t)timestep + 1));
    }
    this->costs[timestep].push_back(cost);
}

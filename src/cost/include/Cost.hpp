#include <string>
#include <vector>

namespace Cost {
    struct Cost {
        std::string category = "";
        double cost = 0;
    };

    class CostTracker {
    public:
        /// @brief Get a vector of total costs per timestep
        /// @return Vector of total costs per timestep
        std::vector<double> getTotals();
        /// @brief Get the vector of costs as-is
        /// @return Vector of vectors of \code{Cost} objects, each element of
        /// the top-level vector representing the timestep during which the cost
        /// was accrued
        std::vector<std::vector<Cost>> getCosts() const { return this->costs; }
        /// @brief Add a cost to the tracker
        /// @param cost The \code{Cost} item to be added
        /// @param timestep The timestep during which this cost is added
        void addCost(Cost cost, int timestep);

    private:
        std::vector<std::vector<Cost>> costs;
    };
} // namespace Cost

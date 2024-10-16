#include "Cost.hpp"
#include <cmath>
#include <iostream>
#include <unordered_map>

namespace cost {
    class CostTracker::CostTrackerIMPL {
    private:
        std::unordered_map<CostCategory, double> costs;

    public:
        CostTrackerIMPL() {
            costs[CostCategory::MISC] = 0.0;
            costs[CostCategory::BEHAVIOR] = 0.0;
            costs[CostCategory::SCREENING] = 0.0;
            costs[CostCategory::LINKING] = 0.0;
            costs[CostCategory::STAGING] = 0.0;
            costs[CostCategory::LIVER] = 0.0;
            costs[CostCategory::TREATMENT] = 0.0;
            costs[CostCategory::BACKGROUND] = 0.0;
        }
        ~CostTrackerIMPL() {};
        double GetTotal() const {
            double sum = 0.0;
            for (auto const &pair : costs) {
                sum += pair.second;
            }
            return sum;
        }

        std::unordered_map<CostCategory, double> GetCosts() const {
            return costs;
        }

        void AddCost(double cost, CostCategory category) {
            this->costs[category] += cost;
        }
    };

    CostTracker::CostTracker() { impl = std::make_unique<CostTrackerIMPL>(); }

    double CostTracker::GetTotal() const { return impl->GetTotal(); }

    std::unordered_map<CostCategory, double> CostTracker::GetCosts() const {
        return impl->GetCosts();
    }

    void CostTracker::AddCost(double cost, CostCategory category) {
        return impl->AddCost(cost, category);
    }

    CostTracker::~CostTracker() = default;

    CostTracker::CostTracker(CostTracker &&p) noexcept = default;
    CostTracker &CostTracker::operator=(CostTracker &&) noexcept = default;

    // dividing discountRate by 12 because discount rates are annual
    // double denominator =
    //     std::pow(1 + discountRate / 12, timestep.first + 1);

    std::ostream &operator<<(std::ostream &os, const CostTracker &cost) {
        for (auto const &pair : cost.GetCosts()) {
            os << pair.first << " " << std::to_string(pair.second) << std::endl;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const CostCategory &inst) {
        switch (inst) {
        case CostCategory::MISC:
            os << "MISC";
            break;
        case CostCategory::BEHAVIOR:
            os << "BEHAVIOR";
            break;
        case CostCategory::SCREENING:
            os << "SCREENING";
            break;
        case CostCategory::LINKING:
            os << "LINKING";
            break;
        case CostCategory::STAGING:
            os << "STAGING";
            break;
        case CostCategory::LIVER:
            os << "LIVER";
            break;
        case CostCategory::TREATMENT:
            os << "TREATMENT";
            break;
        case CostCategory::BACKGROUND:
            os << "BACKGROUND";
            break;
        default:
            os << "N/A";
            break;
        }
        return os;
    }
} // namespace cost

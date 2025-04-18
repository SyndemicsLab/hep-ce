////////////////////////////////////////////////////////////////////////////////
// File: Utility.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-02-24                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Utility.hpp"

#include <algorithm>

namespace utility {
class UtilityTracker::UtilityTrackerIMPL {
private:
    std::unordered_map<UtilityCategory, double> utilities;

public:
    UtilityTrackerIMPL() {
        utilities[UtilityCategory::BEHAVIOR] = 1.0;
        utilities[UtilityCategory::LIVER] = 1.0;
        utilities[UtilityCategory::TREATMENT] = 1.0;
        utilities[UtilityCategory::BACKGROUND] = 1.0;
        utilities[UtilityCategory::HIV] = 1.0;
    }
    ~UtilityTrackerIMPL(){};

    std::pair<double, double> GetUtilities() const {
        double minUtility = std::min(
            std::min(std::min(this->utilities.at(UtilityCategory::BEHAVIOR),
                              this->utilities.at(UtilityCategory::LIVER)),
                     std::min(this->utilities.at(UtilityCategory::TREATMENT),
                              this->utilities.at(UtilityCategory::BACKGROUND))),
            this->utilities.at(UtilityCategory::HIV));
        // avoid numeric overflow by not multiplying if any of the
        // utilities are zero
        double multUtility =
            minUtility == 0 ? 0.0
                            : (this->utilities.at(UtilityCategory::BEHAVIOR) *
                               this->utilities.at(UtilityCategory::LIVER) *
                               this->utilities.at(UtilityCategory::TREATMENT) *
                               this->utilities.at(UtilityCategory::BACKGROUND) *
                               this->utilities.at(UtilityCategory::HIV));
        return {minUtility, multUtility};
    }

    std::unordered_map<UtilityCategory, double> GetRawUtilities() const {
        return utilities;
    }

    void SetUtility(double util, UtilityCategory category) {
        if ((util < 0) || (util > 1)) {
            // raise error
            return;
        }
        utilities[category] = util;
    }
};

UtilityTracker::UtilityTracker() {
    impl = std::make_unique<UtilityTrackerIMPL>();
}

UtilityTracker::~UtilityTracker() = default;
UtilityTracker::UtilityTracker(UtilityTracker &&p) noexcept = default;
UtilityTracker &UtilityTracker::operator=(UtilityTracker &&) noexcept = default;

std::pair<double, double> UtilityTracker::GetUtilities() const {
    return impl->GetUtilities();
}

std::unordered_map<UtilityCategory, double>
UtilityTracker::GetRawUtilities() const {
    return impl->GetRawUtilities();
}

void UtilityTracker::SetUtility(double util, UtilityCategory category) {
    return impl->SetUtility(util, category);
}

std::ostream &operator<<(std::ostream &os, const UtilityTracker &utility) {
    std::pair<double, double> util = utility.GetUtilities();
    os << util.first << " " << std::to_string(util.second) << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const UtilityCategory &uc) {
    switch (uc) {
    case UtilityCategory::BEHAVIOR:
        os << "BEHAVIOR";
        break;
    case UtilityCategory::LIVER:
        os << "LIVER";
        break;
    case UtilityCategory::TREATMENT:
        os << "TREATMENT";
        break;
    case UtilityCategory::BACKGROUND:
        os << "BACKGROUND";
        break;
    case UtilityCategory::HIV:
        os << "HIV";
        break;
    default:
        os << "NA";
        break;
    }
    return os;
}
} // namespace utility

////////////////////////////////////////////////////////////////////////////////
// File: Utility.hpp                                                          //
// Project: HEP-CE                                                            //
// Created Date: 2025-02-24                                                   //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-03                                                  //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_UTILITY_HPP_
#define UTILITY_UTILITY_HPP_

#include <memory>
#include <ostream>
#include <unordered_map>

namespace utility {
    /// @brief Categories for classifying utility items
    enum class UtilityCategory {
        BEHAVIOR = 0,
        LIVER = 1,
        TREATMENT = 2,
        BACKGROUND = 3,
        COUNT = 4
    };
    std::ostream &operator<<(std::ostream &os, const UtilityCategory &uc);

    class UtilityTrackerBase {
    public:
        // get minimal, multiplicative utilities
        virtual std::pair<double, double> GetUtilities() const = 0;

        /// @brief
        virtual std::unordered_map<UtilityCategory, double>
        GetRawUtilities() const = 0;

        /// @brief Set the utility and discounted utility for a specific utility
        /// category
        /// @param util undiscounted utility value
        /// @param category the category whose utility is to be set
        virtual void SetUtility(double util, UtilityCategory category) = 0;
    };

    class UtilityTracker : public UtilityTrackerBase {
    private:
        class UtilityTrackerIMPL;
        std::unique_ptr<UtilityTrackerIMPL> impl;

    public:
        UtilityTracker();
        ~UtilityTracker();

        // Copy Operations
        UtilityTracker(UtilityTracker const &) = delete;
        UtilityTracker &operator=(UtilityTracker const &) = delete;
        UtilityTracker(UtilityTracker &&) noexcept;
        UtilityTracker &operator=(UtilityTracker &&) noexcept;

        std::pair<double, double> GetUtilities() const override;

        std::unordered_map<UtilityCategory, double>
        GetRawUtilities() const override;

        void SetUtility(double util, UtilityCategory category) override;
    };
} // namespace utility
#endif

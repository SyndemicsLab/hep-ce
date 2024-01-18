//===-- Fibrosis.hpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, August 15th 2023, 1:53:22 pm
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_FIBROSIS_HPP_
#define EVENT_FIBROSIS_HPP_

#include "Event.hpp"
#include <algorithm>

/// @brief Namespace containing the Events that occur during the simulation
namespace Event {

    /// @brief Subclass of Event used to Progress Fibrosis Stages
    class FibrosisStaging : public ProbEvent {
    private:
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<Person::Person> person) override;

        /// @brief Aggregate the fibrosis stage testing probabilities for a
        /// given Person object.
        /// @details Fibrosis staging inputs are provided both in the tabular
        /// inputs and in the text-based configuration:
        // clang-format off
        /// | Configuration Input Name | Description |
        /// | ------------------------ | ----------- |
        /// | period                   | Number of months between staging tests.                                                                                                              |
        /// | multitest_result_method  | (Optional) Possible values: latest, maximum. Specifies whether to use the latest test result or the maximum test result when using two test methods. |
        /// | test_one                 | Name of fibrosis staging test one. Must match a column header in the fibrosis staging tabular input.                                                 |
        /// | test_one_cost            | Cost of fibrosis staging test one in USD.                                                                                                            |
        /// | test_two                 | (Optional) Name of fibrosis staging test two. Must match a column header in the fibrosis staging tabular input.                                      |
        /// | test_two_cost            | Cost of fibrosis staging test two in USD.                                                                                                            |
        /// | test_two_eligible_stages | A comma-separated list of which outcomes from test one will be tested again with fibrosis staging test two.                                          |
        // clang-format on
        /// @param table
        /// @param configLookupKey
        /// @return Vector of fibrosis staging outcome probabilities
        std::vector<double> getTransitions(Data::IDataTablePtr table,
                                           std::string configLookupKey);

    public:
        FibrosisStaging(std::mt19937_64 &generator, Data::IDataTablePtr table,
                        Data::Configuration &config,
                        std::shared_ptr<spdlog::logger> logger =
                            std::make_shared<spdlog::logger>("default"),
                        std::string name = std::string("ProbEvent"))
            : ProbEvent(generator, table, config, logger, name) {}
        virtual ~FibrosisStaging() = default;
    };
} // namespace Event

#endif

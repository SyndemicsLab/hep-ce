////////////////////////////////////////////////////////////////////////////////
// File: progression_internals.hpp                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-08-08                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-11                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_FIBROSIS_PROGRESSIONINTERNALS_HPP_
#define HEPCE_EVENT_FIBROSIS_PROGRESSIONINTERNALS_HPP_

// File Header
#include <hepce/event/fibrosis/progression.hpp>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace fibrosis {
class ProgressionImpl : public virtual Progression, public EventBase {
public:
    struct progression_probabilities {
        double f0_to_1 = 0.0;
        double f1_to_2 = 0.0;
        double f2_to_3 = 0.0;
        double f3_to_4 = 0.0;
        double f4_to_d = 0.0;
    };

    using costutilmap_t =
        std::unordered_map<utils::tuple_2i, data::CostUtil, utils::key_hash_2i,
                           utils::key_equal_2i>;

    ProgressionImpl(datamanagement::ModelData &model_data,
                    const std::string &log_name = "console");
    ~ProgressionImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    bool _add_if_identified = false;
    progression_probabilities _probabilities;
    costutilmap_t _cost_data;

    const std::vector<double>
    GetTransitionProbability(const data::FibrosisState &fs) const;

    inline const std::string ProgressionSQL() const {
        return "SELECT hcv_status, fibrosis_state, cost, utility FROM "
               "hcv_impacts;";
    }

    inline const utils::tuple_2i
    TupleBuilder(const model::Person &person) const {
        int hcv_status =
            (person.GetHCVDetails().hcv == data::HCV::kNone) ? 0 : 1;
        int fibrosis_state =
            static_cast<int>(person.GetHCVDetails().fibrosis_state);
        return std::make_tuple(hcv_status, fibrosis_state);
    }

    inline void AddProgressionCost(model::Person &person) {
        AddEventCost(person, _cost_data[TupleBuilder(person)].cost);
    }

    inline void AddProgressionUtility(model::Person &person) {
        AddEventUtility(person, _cost_data[TupleBuilder(person)].util);
    }

    /// @brief Determine if a person accrues a utility and cost.
    /// @param person The person to check if they accrue cost and utility
    inline void ResolveLiverCostAndUtility(model::Person &person) {
        AddProgressionUtility(person);
        data::FibrosisState fs = person.GetHCVDetails().fibrosis_state;

        // fibrosis stages that are clinically presenting - using resources for
        // cirrhosis care
        bool presenting_fib_stages = (fs == data::FibrosisState::kF4 ||
                                      fs == data::FibrosisState::kDecomp);

        // If the sim is set to only accrue costs for identified individuals AND
        // they are identified AND they are not in the clinically presenting
        // fibrosis stages then do not accrue costs.
        if (_add_if_identified &&
            !person.GetScreeningDetails(data::InfectionType::kHcv).identified &&
            !presenting_fib_stages) {
            return;
        }
        AddProgressionCost(person);
    }
};
} // namespace fibrosis
} // namespace event
} // namespace hepce

#endif

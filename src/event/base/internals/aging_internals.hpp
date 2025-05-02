////////////////////////////////////////////////////////////////////////////////
// File: aging_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
#define HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_

#include <hepce/event/base/aging.hpp>

#include <datamanagement/datamanagement.hpp>

#include <hepce/model/costing.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace base {
class AgingImpl : public virtual Aging, public EventBase {
public:
    using agemap_t =
        std::unordered_map<utils::tuple_3i, data::CostUtil, utils::key_hash_3i,
                           utils::key_equal_3i>;
    AgingImpl(datamanagement::ModelData &model_data,
              const std::string &log_name = "console");
    ~AgingImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    agemap_t _age_data;

    inline const std::string BuildSQL() const {
        return "SELECT age_years, gender, drug_behavior, cost, utility "
               "FROM background_impacts;";
    }

    /// @brief Adds person's background cost
    /// @param person The person to whom cost will be added
    void AddBackgroundCostAndUtility(model::Person &person);
};
} // namespace base
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
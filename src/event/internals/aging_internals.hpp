////////////////////////////////////////////////////////////////////////////////
// File: aging_internals.hpp                                                  //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
#define HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_

#include <datamanagement/datamanagement.hpp>

#include <hepce/model/costing.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

#include "event_internals.hpp"

namespace hepce {
namespace event {
class Aging : public EventBase {
public:
    using agemap_t =
        std::unordered_map<utils::tuple_3i, data::CostUtil, utils::key_hash_3i,
                           utils::key_equal_3i>;

    // Constructor
    Aging::Aging(datamanagement::ModelData &md, const std::string &log)
        : EventBase("aging", md, log) {
        LoadData(md);
    }

    // Default Destructor
    ~Aging() = default;

    std::unique_ptr<Event> clone() const override {
        // auto ret = std::make_unique<Aging>();
    }

    // Factory
    static std::unique_ptr<Event> Create(datamanagement::ModelData &model_data,
                                         const std::string &log_name);

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
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
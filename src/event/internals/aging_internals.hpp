////////////////////////////////////////////////////////////////////////////////
// File: aging_internals.hpp                                                  //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
#define HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_

#include <hepce/model/costing.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class Aging : public EventBase {
public:
    using agemap_t =
        std::unordered_map<utils::tuple_3i, data::CostUtil, utils::key_hash_3i,
                           utils::key_equal_3i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    // Constructor
    Aging(const data::Inputs &inputs, const std::string &log)
        : EventBase("aging", inputs, log) {
        LoadData();
    }

    // Default Destructor
    ~Aging() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<Aging>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    agemap_t _age_data;

    void LoadData();

    inline const std::string BuildSQL() const {
        return "SELECT age_years, gender, drug_behavior, cost, utility "
               "FROM background_impacts;";
    }

    /// @brief Adds person's background cost
    /// @param person The person to whom cost will be added
    void AddBackgroundCostAndUtility(model::Person &person) const;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_AGINGINTERNALS_HPP_
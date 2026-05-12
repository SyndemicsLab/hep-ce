////////////////////////////////////////////////////////////////////////////////
// File: hiv_infection_internals.hpp                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_
#define HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class HIVInfection : public virtual EventBase {
public:
    using hivincidencemap_t =
        std::unordered_map<utils::tuple_3i, double, utils::key_hash_3i,
                           utils::key_equal_3i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HIVInfection(const data::Inputs &inputs, const std::string &log)
        : EventBase("hiv_infection", inputs, log) {}

    ~HIVInfection() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HIVInfection>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    hivincidencemap_t _infection_data;

    static void Callback(std::any &storage, const SQLite::Statement &stmt) {
        utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        std::any_cast<hivincidencemap_t>(storage)[tup] =
            stmt.getColumn(3).getDouble();
    }

    inline const std::string HIVIncidenceSQL() const {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "hiv_incidence;";
    }

    std::vector<double> GetInfectionProbability(const model::Person &person) {
        if (_infection_data.empty()) {
            hepce::utils::LogWarning(
                GetLogName(),
                "HIV Infection Probability is Empty. Returning 0.0...");
#ifdef EXIT_ON_WARNING
            std::exit(EXIT_FAILURE);
#endif
            return {0.0};
        }

        int age_years = static_cast<int>(person.GetAge() / 12.0);
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior =
            static_cast<int>(person.GetBehaviorDetails().behavior);
        utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
        double incidence = _infection_data[tup];

        return {incidence};
    }
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_INFECTIONINTERNALS_HPP_
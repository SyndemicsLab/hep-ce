////////////////////////////////////////////////////////////////////////////////
// File: hcv_infection_internals.hpp                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_INFECTIONINTERNALS_HPP_
#define HEPCE_EVENT_HCV_INFECTIONINTERNALS_HPP_

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "base_event_internals.hpp"
namespace hepce {
namespace event {
class HCVInfection : public virtual EventBase {
public:
    using incidencemap_t =
        std::unordered_map<utils::tuple_3i, double, utils::key_hash_3i,
                           utils::key_equal_3i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HCVInfection(const data::Inputs &inputs, const std::string &log)
        : EventBase("hcv_infection", inputs, log),
          _gt3_prob(utils::GetDoubleFromConfig("infection.genotype_three_prob",
                                               inputs)) {
        LoadData();
    }

    ~HCVInfection() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HCVInfection>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    incidencemap_t _infection_data;
    const double _gt3_prob;

    void LoadData();

    static void CallbackInfection(std::any &storage,
                                  const SQLite::Statement &stmt) {
        incidencemap_t *temp = std::any_cast<incidencemap_t>(&storage);
        utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        (*temp)[tup] = stmt.getColumn(3).getDouble();
    }

    inline const std::string IncidenceSQL() const {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "incidence;";
    }

    std::vector<double> GetInfectionProbability(const model::Person &person);
};
} // namespace event
} // namespace hepce

#endif
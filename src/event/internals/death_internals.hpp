////////////////////////////////////////////////////////////////////////////////
// File: death_internals.hpp                                                  //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
#define HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class Death : public virtual EventBase {
public:
    struct BackgroundSmr {
        double back_mort = 0.0;
        double smr = 0.0;
    };
    using backgroundmap_t =
        std::unordered_map<utils::tuple_3i, BackgroundSmr, utils::key_hash_3i,
                           utils::key_equal_3i>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    // Constructor
    Death(const data::Inputs &inputs, const std::string &log)
        : EventBase("death", inputs, log),
          _f4_infected_probability(
              utils::GetDoubleFromConfig("mortality.f4_infected", inputs)),
          _f4_uninfected_probability(
              utils::GetDoubleFromConfig("mortality.f4_uninfected", inputs)),
          _decomp_infected_probability(
              utils::GetDoubleFromConfig("mortality.decomp_infected", inputs)),
          _decomp_uninfected_probability(utils::GetDoubleFromConfig(
              "mortality.decomp_uninfected", inputs)) {
        LoadData();
    }

    // Destructor
    ~Death() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<Death>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    const double _f4_infected_probability;
    const double _f4_uninfected_probability;
    const double _decomp_infected_probability;
    const double _decomp_uninfected_probability;

    double _probability_of_overdose_fatality = 0.0;
    double _fatal_overdose_cost = 0.0;
    double _hiv_mortality_probability = 0.0;

    bool check_overdose = false;
    bool check_hiv = false;

    backgroundmap_t _background_data;

    void LoadData();
    void LoadBackgroundMortality();

    inline const std::string BackgroundMortalitySQL() const {
        std::stringstream sql;
        sql << "SELECT bm.age_years, bm.gender, "
            << "smr.drug_behavior, bm.background_mortality, smr.smr"
            << " FROM smr AS smr"
            << " INNER JOIN background_mortality AS bm ON "
            << "smr.gender = bm.gender;";
        return sql.str();
    }

    inline void Die(model::Person &person,
                    const data::DeathReason &reason) const {
        person.Die(reason);
    }

    bool ReachedMaxAge(model::Person &person) const;

    bool FatalOverdose(model::Person &person, const model::Sampler &sampler);

    bool HivDeath(model::Person &person, const model::Sampler &sampler) const;

    double GetFibrosisMortalityProbability(model::Person &person) const;

    void GetSMRandBackgroundProb(model::Person &person, double &background,
                                 double &smr) const;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: death_internals.hpp                                                  //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-15                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
#define HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_

// File Header
#include <hepce/event/base/death.hpp>

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace base {
class DeathImpl : public virtual Death, public EventBase {
public:
    struct BackgroundSmr {
        double back_mort = 0.0;
        double smr = 0.0;
    };
    using backgroundmap_t =
        std::unordered_map<utils::tuple_3i, BackgroundSmr, utils::key_hash_3i,
                           utils::key_equal_3i>;
    using overdosemap_t =
        std::unordered_map<utils::tuple_2i, double, utils::key_hash_2i,
                           utils::key_equal_2i>;

    DeathImpl(datamanagement::ModelData &model_data,
              const std::string &log_name = "console");
    ~DeathImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    const double _f4_infected_probability;
    const double _f4_uninfected_probability;
    const double _decomp_infected_probability;
    const double _decomp_uninfected_probability;
    double _hiv_mortality_probability = 0.0;

    bool check_overdose = false;
    bool check_hiv = false;

    backgroundmap_t _background_data;
    overdosemap_t _overdose_data;

    inline const std::string OverdoseSQL() const {
        return "SELECT moud, drug_behavior, fatality_probability FROM "
               "overdoses;";
    }

    inline const std::string BackgroundMortalitySQL() const {
        std::stringstream sql;
        sql << "SELECT bm.age_years, bm.gender, "
            << "smr.drug_behavior, bm.background_mortality, smr.smr"
            << " FROM smr AS smr"
            << " INNER JOIN background_mortality AS bm ON "
            << "smr.gender = bm.gender;";
        return sql.str();
    }

    inline void Die(model::Person &person, const data::DeathReason &reason) {
        person.Die(reason);
    }

    void LoadOverdoseData(datamanagement::ModelData &model_data);

    void LoadBackgroundMortality(datamanagement::ModelData &model_data);

    bool ReachedMaxAge(model::Person &person);

    bool FatalOverdose(model::Person &person, model::Sampler &sampler);

    bool HivDeath(model::Person &person, model::Sampler &sampler);

    double GetFibrosisMortalityProbability(model::Person &person);

    void GetSMRandBackgroundProb(model::Person &person,
                                 double &backgroundMortProb, double &smr);
};
} // namespace base
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: death_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
#define HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_

#include <hepce/event/base/death.hpp>

#include "internals/event_internals.hpp"
#include <hepce/utils/config.hpp>
#include <hepce/utils/pair_hashing.hpp>

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

    int Execute(model::Person &person, model::Sampler &sampler) override;

private:
    double _f4_infected_probability;
    double _f4_uninfected_probability;
    double _decomp_infected_probability;
    double _decomp_uninfected_probability;

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

    inline const std::string OverdoseTableSQL() const {
        return "SELECT name FROM sqlite_master WHERE type = ? AND name = ?;";
    }

    inline void Die(model::Person &person, const data::DeathReason &reason) {
        person.Die(reason);
    }

    int CheckOverdoseTable(datamanagement::ModelData &model_data);

    int LoadOverdoseData(datamanagement::ModelData &model_data);

    int LoadBackgroundMortality(datamanagement::ModelData &model_data);

    bool DeathImpl::ReachedMaxAge(model::Person &person);

    bool FatalOverdose(model::Person &person, model::Sampler &decider);

    void GetFibrosisMortalityProb(model::Person &person, double &prob);

    void GetSMRandBackgroundProb(model::Person &person,
                                 double &backgroundMortProb, double &smr);
};
} // namespace base
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: death_internals.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
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

    DeathImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
              const std::string &log_name = "console") {
        _f4_infected_probability =
            utils::GetDoubleFromConfig("mortality.f4_infected", dm);
        _f4_uninfected_probability =
            utils::GetDoubleFromConfig("mortality.f4_uninfected", dm);
        _decomp_infected_probability =
            utils::GetDoubleFromConfig("mortality.decomp_infected", dm);
        _decomp_uninfected_probability =
            utils::GetDoubleFromConfig("mortality.decomp_uninfected", dm);

        LoadOverdoseData(dm);
        LoadBackgroundMortality(dm);
    }
    ~DeathImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    double _f4_infected_probability;
    double _f4_uninfected_probability;
    double _decomp_infected_probability;
    double _decomp_uninfected_probability;

    backgroundmap_t _background_data;
    overdosemap_t _overdose_data;

    static int CallbackOverdose(void *storage, int count, char **data,
                                char **columns) {
        utils::tuple_2i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
        (*((overdosemap_t *)storage))[tup] = utils::SToDPositive(data[2]);
        return 0;
    }

    static int CallbackBackground(void *storage, int count, char **data,
                                  char **columns) {
        utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        struct BackgroundSmr temp = {utils::SToDPositive(data[3]),
                                     utils::SToDPositive(data[4])};
        (*((backgroundmap_t *)storage))[tup] = temp;
        return 0;
    }

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

    int CheckOverdoseTable(std::shared_ptr<datamanagement::DataManagerBase> dm);

    int LoadOverdoseData(std::shared_ptr<datamanagement::DataManagerBase> dm);

    int LoadBackgroundMortality(
        std::shared_ptr<datamanagement::DataManagerBase> dm);

    bool DeathImpl::ReachedMaxAge(model::Person &person);

    bool FatalOverdose(model::Person &person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       model::Sampler &decider);

    void GetFibrosisMortalityProb(
        model::Person &person,
        std::shared_ptr<datamanagement::DataManagerBase> dm, double &prob);

    void
    GetSMRandBackgroundProb(model::Person &person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            double &backgroundMortProb, double &smr);
};
} // namespace base
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_DEATHINTERNALS_HPP_
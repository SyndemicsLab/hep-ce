////////////////////////////////////////////////////////////////////////////////
// File: treatment_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
#define HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_

#include <hepce/event/hcv/treatment.hpp>

#include "../../internals/treatment_internals.hpp"
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace hcv {
class TreatmentImpl : public virtual Treatment, public TreatmentBase {
public:
    struct TreatmentSQLData {
        int duration = 0;
        double cost = 0.0;
        double svr_probability = 0.0;
        double toxicity_probability = 0.0;
        double withdrawal_probability = 0.0;
    };

    using hcvtreatmentmap_t =
        std::unordered_map<utils::tuple_3i, TreatmentSQLData,
                           utils::key_hash_3i, utils::key_equal_3i>;

    TreatmentImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");
    ~TreatmentImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    hcvtreatmentmap_t _treatment_sql_data;

    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHcv;
    }

    static void Callback(std::any &storage, const SQLite::Statement &stmt) {
        hcvtreatmentmap_t *temp = std::any_cast<hcvtreatmentmap_t>(&storage);
        utils::tuple_3i key = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        (*temp)[key] = {
            stmt.getColumn(3).getInt(), stmt.getColumn(4).getDouble(),
            stmt.getColumn(5).getDouble(), stmt.getColumn(6).getDouble(),
            stmt.getColumn(7).getDouble()};
    }

    inline const std::string TreatmentSQL() const {
        return "SELECT salvage, genotype_three, cirrhotic, duration, cost, "
               "svr_prob_if_completed, toxicity_prob_if_withdrawal, withdrawal "
               "FROM treatments;";
    }

    utils::tuple_3i GetTreatmentThruple(const model::Person &person) const {
        int geno3 = (person.GetHCVDetails().is_genotype_three) ? 1 : 0;
        int cirr = (person.IsCirrhotic()) ? 1 : 0;
        int salvage =
            static_cast<int>(person.GetTreatmentDetails(GetInfectionType())
                                 .in_salvage_treatment);
        return std::make_tuple(salvage, geno3, cirr);
    }

    inline void ResetUtility(model::Person &person) const override {
        person.SetUtility(1.0, GetEventUtilityCategory());
    }

    bool Withdraws(model::Person &person, model::Sampler &sampler) {
        if (sampler.GetDecision(
                {_treatment_sql_data[GetTreatmentThruple(person)]
                     .withdrawal_probability}) == 0) {
            person.AddWithdrawal(GetInfectionType());
            QuitEngagement(person);
            return true;
        }
        return false;
    }

    void CheckIfExperienceToxicity(model::Person &person,
                                   model::Sampler &sampler) {
        if (sampler.GetDecision(
                {_treatment_sql_data[GetTreatmentThruple(person)]
                     .toxicity_probability}) == 1) {
            return;
        }
        person.AddToxicReaction(GetInfectionType());
        AddEventCost(person, GetTreatmentCosts().toxicity);
        AddEventUtility(person, GetTreatmentUtilities().toxicity);
    }

    bool InitiateTreatment(model::Person &person,
                           model::Sampler &sampler) const;

    inline int DecideIfPersonAchievesSVR(const model::Person &person,
                                         model::Sampler &sampler) {
        return sampler.GetDecision(
            {_treatment_sql_data[GetTreatmentThruple(person)].svr_probability});
    }

    inline int GetTreatmentDuration(const model::Person &person) {
        return _treatment_sql_data[GetTreatmentThruple(person)].duration;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
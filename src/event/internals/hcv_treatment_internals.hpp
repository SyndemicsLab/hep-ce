////////////////////////////////////////////////////////////////////////////////
// File: hcv_treatment_internals.hpp                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
#define HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_

#include "base_treatment_internals.hpp"

#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
class HCVTreatment : public virtual TreatmentBase {
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

    using hcvtreatmentinitmap_t = std::unordered_map<int, double>;

    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HCVTreatment(const data::Inputs &inputs, const std::string &log)
        : TreatmentBase("hcv_treatment", inputs, log) {
        LoadData();
    }
    ~HCVTreatment() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HCVTreatment>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    hcvtreatmentmap_t _treatment_sql_data;
    hcvtreatmentinitmap_t _treatment_init_sql_data;

    void LoadData();

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

    inline const std::string TreatmentInitializationSQL() const {
        return "SELECT pregnancy_state, treatment_initiation "
               "FROM treatment_initiations;";
    }

    static void CallbackTreatmentInitialization(std::any &storage,
                                                const SQLite::Statement &stmt) {
        hcvtreatmentinitmap_t *temp =
            std::any_cast<hcvtreatmentinitmap_t>(&storage);
        int key = stmt.getColumn(0).getInt();
        (*temp)[key] = stmt.getColumn(1).getDouble();
    }

    inline const std::string TreatmentSQL() const {
        return "SELECT salvage, genotype_three, cirrhotic, "
               "duration, cost, svr_prob_if_completed, "
               "toxicity_prob_if_withdrawal, withdrawal "
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
        person.SetUtility(1.0, GetUtilityCategory());
    }

    bool Withdraws(model::Person &person, const model::Sampler &sampler) {
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
                                   const model::Sampler &sampler) {
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
                           const model::Sampler &sampler) const;

    inline int DecideIfPersonAchievesSVR(const model::Person &person,
                                         const model::Sampler &sampler) {
        return sampler.GetDecision(
            {_treatment_sql_data[GetTreatmentThruple(person)].svr_probability});
    }

    inline int GetTreatmentDuration(const model::Person &person) {
        return _treatment_sql_data[GetTreatmentThruple(person)].duration;
    }
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
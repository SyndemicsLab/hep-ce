////////////////////////////////////////////////////////////////////////////////
// File: treatment_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
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
class TreatmentImpl : public virtual hcv::Treatment,
                      public event::TreatmentBase {
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

    int Execute(model::Person &person, model::Sampler &sampler) override;

private:
    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHcv;
    }

    hcvtreatmentmap_t _treatment_sql_data;

    // user-provided values
    double retreatment_cost;
    double treatment_utility;

    static void Callback(std::any &storage, const SQLite::Statement &stmt) {
        utils::tuple_3i key = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        std::any_cast<hcvtreatmentmap_t>(storage)[key] = {
            stmt.getColumn(3).getInt(), stmt.getColumn(4).getDouble(),
            stmt.getColumn(5).getDouble(), stmt.getColumn(6).getDouble(),
            stmt.getColumn(7).getDouble()};
    }

    inline const std::string TreatmentSQL() const {
        return "SELECT retreatment, genotype_three, cirrhotic, duration, cost, "
               "svr_prob_if_completed, toxicity_prob_if_withdrawal, withdrawal "
               "FROM treatments;";
    }

    utils::tuple_3i GetTreatmentThruple(const model::Person &person) const {
        int geno3 = (person.GetHCVDetails().is_genotype_three) ? 1 : 0;
        int cirr = (person.IsCirrhotic()) ? 1 : 0;
        int retreatment = static_cast<int>(
            person.GetTreatmentDetails(GetInfectionType()).retreatment);
        return std::make_tuple(retreatment, geno3, cirr);
    }

    void ChargeCostOfCourse(model::Person &person) {
        ChargeCost(person,
                   _treatment_sql_data[GetTreatmentThruple(person)].cost);
        SetEventUtility(treatment_utility);
        AddEventUtility(person);
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
        ChargeCost(person, GetTreatmentCosts().toxicity);
        SetEventUtility(GetTreatmentUtilities().toxicity);
        AddEventUtility(person);
    }

    bool InitiateTreatment(model::Person &person, model::Sampler &sampler) {
        // person initiates treatment -- set treatment initiation values
        if (IsEligible(person) &&
            (sampler.GetDecision(
                 {GetTreatmentProbabilities().initialization}) == 0)) {
            person.InitiateTreatment(GetInfectionType());
            return true;
        }
        return false;
    }

    int DecideIfPersonAchievesSVR(const model::Person &person,
                                  model::Sampler &sampler) {
        return sampler.GetDecision(
            {_treatment_sql_data[GetTreatmentThruple(person)].svr_probability});
    }

    inline int GetTreatmentDuration(const model::Person &person) {
        return _treatment_sql_data[GetTreatmentThruple(person)].duration;
    }

    int LoadTreatmentSQLData(datamanagement::ModelData &model_data) {
        std::any storage = _treatment_sql_data;
        model_data.GetDBSource("inputs").Select(TreatmentSQL(), Callback,
                                                storage);
        _treatment_sql_data = std::any_cast<hcvtreatmentmap_t>(storage);
        if (_treatment_sql_data.empty()) {
            // Warn Empty
        }
        return 0;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
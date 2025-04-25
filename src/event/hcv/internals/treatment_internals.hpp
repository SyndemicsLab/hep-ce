////////////////////////////////////////////////////////////////////////////////
// File: treatment_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
#define HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_

#include <hepce/event/hcv/treatment.hpp>

#include "internals/treatment_internals.hpp"
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

    TreatmentImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");
    ~TreatmentImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::HCV;
    }

    hcvtreatmentmap_t _treatment_sql_data;

    // user-provided values
    double retreatment_cost;
    double treatment_utility;

    static int Callback(void *storage, int count, char **data, char **columns) {
        utils::tuple_3i key = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((hcvtreatmentmap_t *)storage))[key] = {
            std::stoi(data[3]), utils::SToDPositive(data[4]),
            utils::SToDPositive(data[5]), utils::SToDPositive(data[6]),
            utils::SToDPositive(data[7])};
        return 0;
    }

    std::string TreatmentSQL() {
        return "SELECT retreatment, genotype_three, cirrhotic, duration, cost, "
               "svr_prob_if_completed, toxicity_prob_if_withdrawal, withdrawal "
               "FROM treatments;";
    }

    utils::tuple_3i GetTreatmentThruple(const model::Person &person) const {
        int geno3 = (person.IsGenotypeThree()) ? 1 : 0;
        int cirr = (person.IsCirrhotic()) ? 1 : 0;
        int retreatment = static_cast<int>(person.IsInRetreatment());
        return std::make_tuple(retreatment, geno3, cirr);
    }

    void ChargeCostOfCourse(model::Person &person) {
        double course_cost =
            _treatment_sql_data[GetTreatmentThruple(person)].cost;
        ChargeCost(person, course_cost);
        SetUtil(treatment_utility);
        AddEventUtility(person);
    }

    bool Withdraws(model::Person &person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   model::Sampler &sampler) {
        if (sampler.GetDecision(
                {_treatment_sql_data[GetTreatmentThruple(person)]
                     .withdrawal_probability}) == 0) {
            person.AddWithdrawal();
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
        person.AddToxicReaction();
        ChargeCost(person, GetTreatmentCosts().toxicity);
        SetUtil(GetTreatmentUtilities().toxicity);
        AddEventUtility(person);
    }

    bool InitiateTreatment(model::Person &person, model::Sampler &sampler) {
        // person initiates treatment -- set treatment initiation values
        if (IsEligible(person) &&
            (sampler.GetDecision(
                 {GetTreatmentProbabilities().initialization}) == 0)) {
            person.InitiateTreatment();
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

    int
    LoadTreatmentSQLData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(TreatmentSQL(), Callback,
                                          &_treatment_sql_data, error);
        if (rc != 0) {
            spdlog::get("main")->error("Error extracting Treatment Data "
                                       "from treatments! Error Message: {}",
                                       error);
        }
        return rc;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_TREATMENTINTERNALS_HPP_
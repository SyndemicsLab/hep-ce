////////////////////////////////////////////////////////////////////////////////
// File: treatment_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Mo Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_TREATMENTINTERNALS_HPP_
#define HEPCE_EVENT_HIV_TREATMENTINTERNALS_HPP_

#include <hepce/event/hiv/treatment.hpp>

#include <hepce/utils/pair_hashing.hpp>

#include "../../internals/treatment_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
class TreatmentImpl : public virtual hiv::Treatment,
                      public event::TreatmentBase {
public:
    struct HivTreatmentData {
        double course_cost = 0.0;
        double toxicity_prob = 0.0;
        double withdrawal_prob = 0.0;
        int suppression_months = -1;
        int restore_high_cd4_months = -1;
    };

    using hivtreatmentmap_t = std::unordered_map<std::string, HivTreatmentData>;

    using hivutilitymap_t =
        std::unordered_map<utils::tuple_2i, double, utils::key_hash_2i,
                           utils::key_equal_2i>;

    TreatmentImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");
    ~TreatmentImpl() = default;

    int Execute(model::Person &person, model::Sampler &sampler) override;

private:
    hivutilitymap_t _utility_data;
    hivtreatmentmap_t _treatment_sql_data;
    std::string _course_name;

    inline const std::string HIVTreatmentSQL() const {
        return "SELECT course, cost, toxicity_prob, withdrawal_prob, "
               "months_to_suppression, months_to_high_cd4 FROM hiv_treatments;";
    }

    inline const std::string HIVUtilitySQL() const {
        return "SELECT HIV_trt, CD4_count, utility FROM HIV_table;";
    }

    static void CallbackTreatment(std::any &storage,
                                  const SQLite::Statement &stmt) {
        std::any_cast<hivtreatmentmap_t>(
            storage)[stmt.getColumn(0).getText()] = {
            stmt.getColumn(1).getDouble(), stmt.getColumn(2).getDouble(),
            stmt.getColumn(3).getDouble(), stmt.getColumn(4).getInt(),
            stmt.getColumn(5).getInt()};
    }

    static void CallbackUtility(std::any &storage,
                                const SQLite::Statement &stmt) {
        // either ON or OFF treatment
        int hiv_treatment = (stmt.getColumn(0).getText() == "ON") ? 1 : 0;
        // either high or low CD4 count
        int cd4_count = (stmt.getColumn(1).getText() == "high") ? 1 : 0;
        utils::tuple_2i key = std::make_tuple(hiv_treatment, cd4_count);
        std::any_cast<hivutilitymap_t>(storage)[key] =
            stmt.getColumn(2).getDouble();
    }

    void LoadUtilityData(datamanagement::ModelData &model_data) {
        std::any storage = _utility_data;
        model_data.GetDBSource("inputs").Select(HIVUtilitySQL(),
                                                CallbackUtility, storage);
        _utility_data = std::any_cast<hivutilitymap_t>(storage);
    }

    void LoadCourseData(datamanagement::ModelData &model_data) {
        std::any storage = _treatment_sql_data;
        model_data.GetDBSource("inputs").Select(HIVTreatmentSQL(),
                                                CallbackTreatment, storage);
        _treatment_sql_data = std::any_cast<hivtreatmentmap_t>(storage);
    }

    bool IsLowCD4(model::Person &person) {
        if ((person.GetHIV() == data::HIV::LOUN) ||
            (person.GetHIV() == data::HIV::LOSU)) {
            return true;
        }
        return false;
    }

    void RestoreHighCD4(model::Person &person) {
        if (person.GetHIV() == data::HIV::LOUN) {
            person.SetHIV(data::HIV::HIUN);
        } else if (person.GetHIV() == data::HIV::LOSU) {
            person.SetHIV(data::HIV::HISU);
        }
    }

    bool InitiateTreatment(model::Person &person, model::Sampler &sampler) {
        // do not start treatment if the individual is not eligible
        if (!IsEligible(person)) {
            return false;
        }
        if (sampler.GetDecision({GetTreatmentProbabilities().initialization}) ==
            0) {
            person.InitiateTreatment();
            return true;
        }
        return false;
    }

    bool Withdraws(model::Person &person, model::Sampler &sampler) {
        if (_treatment_sql_data[_course_name].withdrawal_prob == 0) {
            // spdlog::get("main")->warn(
            //     "HIV treatment withdrawal probability is "
            //     "0. If this isn't intended, check your inputs!");
        }

        if (sampler.GetDecision(
                {_treatment_sql_data[_course_name].withdrawal_prob}) == 0) {
            person.AddWithdrawal();
            QuitEngagement(person);
            return true;
        }
        return false;
    }

    void ChargeCostOfCourse(model::Person &person) {
        ChargeCost(person, _treatment_sql_data[_course_name].course_cost);
        this->SetUtility(person);
    }

    void CheckIfExperienceToxicity(model::Person &person,
                                   model::Sampler &sampler) {
        if (sampler.GetDecision(
                {_treatment_sql_data[_course_name].toxicity_prob}) == 1) {
            return;
        }
        person.AddToxicReaction();
        ChargeCost(person, GetTreatmentCosts().toxicity);
        SetUtil(GetTreatmentUtilities().toxicity);
        AddEventUtility(person);
    }

    /// @brief Used to set person's HIV utility after engaging with treatment
    /// @param
    void SetUtility(model::Person &person) {
        utils::tuple_2i key;
        switch (person.GetHIV()) {
        case data::HIV::HISU:
        case data::HIV::HIUN:
            key = std::make_tuple(1, 1);
            SetUtil(_utility_data[key]);
            AddEventUtility(person);
            break;
        case data::HIV::LOSU:
        case data::HIV::LOUN:
            key = std::make_tuple(1, 0);
            SetUtil(_utility_data[key]);
            AddEventUtility(person);
            break;
        default:
            break;
        }
    }

    /// @brief Used to reset person's HIV utility after discontinuing treatment
    /// @param
    void ResetUtility(model::Person &person) {
        utils::tuple_2i key;
        switch (person.GetHIV()) {
        case data::HIV::HISU:
        case data::HIV::HIUN:
            key = std::make_tuple(0, 1);
            SetUtil(_utility_data[key]);
            AddEventUtility(person);
            break;
        case data::HIV::LOSU:
        case data::HIV::LOUN:
            key = std::make_tuple(0, 0);
            SetUtil(_utility_data[key]);
            AddEventUtility(person);
            break;
        default:
            break;
        }
    }

    void ApplySuppression(model::Person &person) {
        switch (person.GetHIV()) {
        case data::HIV::HIUN:
            person.SetHIV(data::HIV::HISU);
            break;
        case data::HIV::LOUN:
            person.SetHIV(data::HIV::LOSU);
            break;
        default:
            break;
        }
    }

    void LoseSuppression(model::Person &person) {
        switch (person.GetHIV()) {
        case data::HIV::HISU:

            person.SetHIV(data::HIV::HIUN);
            break;
        case data::HIV::LOSU:
            person.SetHIV(data::HIV::LOUN);
            break;
        default:
            break;
        }
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_TREATMENTINTERNALS_HPP_
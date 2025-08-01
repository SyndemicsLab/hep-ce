////////////////////////////////////////////////////////////////////////////////
// File: treatment_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-21                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-06                                                  //
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
class TreatmentImpl : public virtual Treatment, public TreatmentBase {
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

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    hivutilitymap_t _utility_data;
    hivtreatmentmap_t _treatment_sql_data;
    std::string _course_name;

    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHiv;
    }

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

    inline bool IsLowCD4(model::Person &person) {
        if ((person.GetHIVDetails().hiv == data::HIV::kLoUn) ||
            (person.GetHIVDetails().hiv == data::HIV::kLoSu)) {
            return true;
        }
        return false;
    }

    inline void RestoreHighCD4(model::Person &person) {
        if (person.GetHIVDetails().hiv == data::HIV::kLoUn) {
            person.SetHIV(data::HIV::kHiUn);
        } else if (person.GetHIVDetails().hiv == data::HIV::kLoSu) {
            person.SetHIV(data::HIV::kHiSu);
        }
    }

    void ResetUtility(model::Person &person) const override;

    bool InitiateTreatment(model::Person &person, model::Sampler &sampler);

    bool Withdraws(model::Person &person, model::Sampler &sampler);

    void CheckIfExperienceToxicity(model::Person &person,
                                   model::Sampler &sampler);

    /// @brief Used to set person's HIV utility after engaging with treatment
    /// @param
    void SetTreatmentUtility(model::Person &person);

    /// @brief Used to reset person's HIV utility after discontinuing treatment
    /// @param
    void ResetUtility(model::Person &person);

    void ApplySuppression(model::Person &person);

    void LoseSuppression(model::Person &person);
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_TREATMENTINTERNALS_HPP_
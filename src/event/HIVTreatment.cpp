////////////////////////////////////////////////////////////////////////////////
// File: HIVTreatment.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-17                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVTreatment.hpp"
#include "Decider.hpp"
#include "Treatment.hpp"

namespace event {
class HIVTreatmentIMPL : public TreatmentIMPL {
private:
    using hivutilitymap_t =
        std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                           Utils::key_equal_2i>;
    hivutilitymap_t utility_data;

    // user-provided values
    struct hivtreatmentdata {
        // cost of the sole HIV treatment
        double course_cost = 0.0;
        // probability of toxicity from treatment
        double toxicity_prob = 0.0;
        // probability of withdrawal from treatment
        double withdrawal_prob = 0.0;
        // months in treatment required to achieve suppression
        int suppression_months = -1;
        // number of months to go from low to high CD4 count
        int restore_high_cd4_months = -1;
    };
    hivtreatmentdata treatment_data;

    std::string HIVTreatmentSQL(std::string course) {
        return "SELECT cost, toxicity_prob, withdrawal_prob, "
               "months_to_suppression, months_to_high_cd4 FROM hiv_treatments "
               "WHERE course == '" +
               course + "';";
    }

    std::string HIVUtilitySQL() {
        return "SELECT HIV_trt, CD4_count, utility FROM HIV_table;";
    }

    static int callback_hivtreatment(void *storage, int count, char **data,
                                     char **columns) {
        // [TODO] write an error if more than one row of data is selected
        (*((hivtreatmentdata *)storage)) = {
            Utils::stod_positive(data[0]), Utils::stod_positive(data[1]),
            Utils::stod_positive(data[2]), std::stoi(data[3]),
            std::stoi(data[4])};
        return 0;
    }

    static int callback_hivutility(void *storage, int count, char **data,
                                   char **columns) {
        // either ON or OFF treatment
        int hiv_treatment = (data[0] == "ON") ? 1 : 0;
        // either high or low CD4 count
        int cd4_count = (data[1] == "high") ? 1 : 0;
        Utils::tuple_2i key = std::make_tuple(hiv_treatment, cd4_count);
        (*((hivutilitymap_t *)storage))[key] = Utils::stod_positive(data[2]);
        return 0;
    }

    void LoadUtilityData(datamanagement::ModelData &model_data) {
        std::string error;
        int rc = dm->SelectCustomCallback(
            HIVUtilitySQL(), this->callback_hivutility, &utility_data, error);
        if (rc != 0) {
            spdlog::get("main")->error("Error extracting HIV utility data from "
                                       "`HIV_table'! Error Message: {}",
                                       error);
        }
    }

    void LoadCourseData(std::string course,
                        datamanagement::ModelData &model_data) {
        std::string error;
        int rc = dm->SelectCustomCallback(HIVTreatmentSQL(course),
                                          this->callback_hivtreatment,
                                          &treatment_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error extracting HIV treatment data "
                "from `hiv_treatments'! Error Message: {}",
                error);
        }
    }

    bool IsLowCD4(std::shared_ptr<person::PersonBase> person) {
        if ((person->GetHIV() == person::HIV::LOUN) ||
            (person->GetHIV() == person::HIV::LOSU)) {
            return true;
        }
        return false;
    }

    void RestoreHighCD4(std::shared_ptr<person::PersonBase> person) {
        if (person->GetHIV() == person::HIV::LOUN) {
            person->SetHIV(person::HIV::HIUN);
        } else if (person->GetHIV() == person::HIV::LOSU) {
            person->SetHIV(person::HIV::HISU);
        }
    }

    bool InitiateTreatment(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<stats::DeciderBase> decider) {
        // do not start treatment if the individual is not eligible
        if (!IsEligible(person)) {
            return false;
        }
        if (decider->GetDecision({this->treatment_init_probability}) == 0) {
            person->InitiateTreatment(this->INF_TYPE);
            return true;
        }
        return false;
    }

    bool Withdraws(std::shared_ptr<person::PersonBase> person,
                   datamanagement::ModelData &model_data,
                   std::shared_ptr<stats::DeciderBase> decider) {
        if (this->treatment_data.withdrawal_prob == 0) {
            spdlog::get("main")->warn(
                "HIV treatment withdrawal probability is "
                "0. If this isn't intended, check your inputs!");
        }

        if (decider->GetDecision({this->treatment_data.withdrawal_prob}) == 0) {
            person->AddWithdrawal(this->INF_TYPE);
            QuitEngagement(person);
            return true;
        }
        return false;
    }

    void ChargeCostOfCourse(std::shared_ptr<person::PersonBase> person) {
        ChargeCost(person, this->treatment_data.course_cost);
        this->SetUtility(person);
    }

    void
    CheckIfExperienceToxicity(std::shared_ptr<person::PersonBase> person,
                              std::shared_ptr<stats::DeciderBase> decider) {
        if (decider->GetDecision({this->treatment_data.toxicity_prob}) == 1) {
            return;
        }
        person->AddToxicReaction(INF_TYPE);
        ChargeCost(person, this->toxicity_cost);
        person->SetUtility(this->toxicity_utility, this->UTIL_CATEGORY);
    }

    /// @brief Used to set person's HIV utility after engaging with treatment
    /// @param
    void SetUtility(std::shared_ptr<person::PersonBase> person) {
        Utils::tuple_2i key;
        switch (person->GetHIV()) {
        case person::HIV::HISU:
        case person::HIV::HIUN:
            key = std::make_tuple(1, 1);
            person->SetUtility(utility_data[key], this->UTIL_CATEGORY);
            break;
        case person::HIV::LOSU:
        case person::HIV::LOUN:
            key = std::make_tuple(1, 0);
            person->SetUtility(utility_data[key], this->UTIL_CATEGORY);
            break;
        default:
            break;
        }
    }

    /// @brief Used to reset person's HIV utility after discontinuing treatment
    /// @param
    void ResetUtility(std::shared_ptr<person::PersonBase> person) {
        Utils::tuple_2i key;
        switch (person->GetHIV()) {
        case person::HIV::HISU:
        case person::HIV::HIUN:
            key = std::make_tuple(0, 1);
            person->SetUtility(utility_data[key], this->UTIL_CATEGORY);
            break;
        case person::HIV::LOSU:
        case person::HIV::LOUN:
            key = std::make_tuple(0, 0);
            person->SetUtility(utility_data[key], this->UTIL_CATEGORY);
            break;
        default:
            break;
        }
    }

    void ApplySuppression(std::shared_ptr<person::PersonBase> person) {
        switch (person->GetHIV()) {
        case person::HIV::HIUN:
            person->SetHIV(person::HIV::HISU);
            break;
        case person::HIV::LOUN:
            person->SetHIV(person::HIV::LOSU);
            break;
        default:
            break;
        }
    }

    void LoseSuppression(std::shared_ptr<person::PersonBase> person) {
        switch (person->GetHIV()) {
        case person::HIV::HISU:

            person->SetHIV(person::HIV::HIUN);
            break;
        case person::HIV::LOSU:
            person->SetHIV(person::HIV::LOUN);
            break;
        default:
            break;
        }
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // Ensure that Person is linked to care
        if (person->GetLinkState(INF_TYPE) != person::LinkageState::LINKED) {
            return;
        }

        // Determine if Person is lost to follow up before beginning treatment
        if (LostToFollowUp(person, decider)) {
            return;
        }

        // Charge the cost of the visit
        ChargeCost(person, this->treatment_cost);

        // Determine if Person initiates treatment
        if (!person->HasInitiatedTreatment(INF_TYPE) &&
            !InitiateTreatment(person, decider)) {
            return;
        }

        // Charge the cost of the treatment
        ChargeCostOfCourse(person);

        // Check if treatment causes toxicity
        CheckIfExperienceToxicity(person, decider);

        // Determine if Person withdraws from care
        if (Withdraws(person, dm, decider)) {
            // if achieving suppression through treatment, it instantaneously
            // ends
            this->LoseSuppression(person);
        }

        // apply suppression if the person has been in treatment long enough
        // must equal suppression months so that this is only triggered at the
        // time of having been in treatment long enough
        if (person->GetTimeSinceTreatmentInitiation(INF_TYPE) ==
            this->treatment_data.suppression_months) {
            this->ApplySuppression(person);
        }

        // if person is has a low CD4/T-cell count and has been on treatment
        // long enough, restore their CD4 count to high
        if (IsLowCD4(person) &&
            person->GetTimeSinceTreatmentInitiation(INF_TYPE) ==
                this->treatment_data.restore_high_cd4_months) {
            this->RestoreHighCD4(person);
        }
    }

    HIVTreatmentIMPL(datamanagement::ModelData &model_data)
        : TreatmentIMPL(dm) {
        this->INF_TYPE = person::InfectionType::HIV;
        this->COST_CATEGORY = cost::CostCategory::HIV;
        this->UTIL_CATEGORY = utility::UtilityCategory::HIV;

        // user input loading
        this->lost_to_follow_up_probability =
            Utils::GetDoubleFromConfig("hiv_treatment.ltfu_probability", dm);
        this->treatment_cost =
            Utils::GetDoubleFromConfig("hiv_treatment.treatment_cost", dm);
        this->treatment_init_probability = Utils::GetDoubleFromConfig(
            "hiv_treatment.treatment_initiation", dm);
        this->toxicity_cost =
            Utils::GetDoubleFromConfig("hiv_treatment.tox_cost", dm);
        this->toxicity_utility =
            Utils::GetDoubleFromConfig("hiv_treatment.tox_utility", dm);

        // load treatment course data - course defined in sim.conf
        std::string course_name =
            Utils::GetStringFromConfig("hiv_treatment.course", dm);
        LoadCourseData(course_name, dm);
        LoadUtilityData(dm);
    }
};

HIVTreatment::HIVTreatment(datamanagement::ModelData &model_data) {
    impl = std::make_unique<HIVTreatmentIMPL>(dm);
}
HIVTreatment::~HIVTreatment() = default;
HIVTreatment::HIVTreatment(HIVTreatment &&) noexcept = default;
HIVTreatment &HIVTreatment::operator=(HIVTreatment &&) noexcept = default;

void HIVTreatment::DoEvent(std::shared_ptr<person::PersonBase> person,
                           datamanagement::ModelData &model_data,
                           std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event

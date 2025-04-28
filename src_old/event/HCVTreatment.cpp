////////////////////////////////////////////////////////////////////////////////
// File: HCVTreatment.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "HCVTreatment.hpp"
#include "Decider.hpp"
#include "Treatment.hpp"

namespace event {
class HCVTreatmentIMPL : public TreatmentIMPL {
public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // 0. Verify the person is linked before starting treatment
        if (person->GetLinkState() != person::LinkageState::kLinked) {
            return;
        }

        // 1. Check if the Person is Lost To Follow Up (LTFU)
        if (LostToFollowUp(person, decider)) {
            return;
        }

        // 2. Charge the Cost of the Visit (varies if this is retreatment)
        int num_treatments =
            person->GetWithdrawals() + person->GetCompletedTreatments();
        double visit_cost = (num_treatments > 0) ? this->retreatment_cost
                                                 : this->treatment_cost;
        ChargeCost(person, visit_cost);

        // 3. Attempt to start primary treatment, if already on treatment
        // nothing happens
        if (!person->HasInitiatedTreatment() &&
            !InitiateTreatment(person, decider)) {
            return;
        }

        // 4. Charge the person for the Course they are on
        ChargeCostOfCourse(person);

        // 5. Check if the person experiences toxicity
        CheckIfExperienceToxicity(person, decider);

        // 6. Determine if the person withdraws from the treatment
        if (Withdraws(person, dm, decider)) {
            return;
        }

        // 7. Determine if the person has been treated long enough and, if so,
        // if they achieve SVR
        int duration = GetTreatmentDuration(person);
        if (person->GetTimeSinceTreatmentInitiation() == duration) {
            person->AddCompletedTreatment();
            int decision = DecideIfPersonAchievesSVR(person, decider);
            if (decision == 0) {
                person->AddSVR();
                person->ClearHCV();
                person->ClearDiagnosis();
                this->QuitEngagement(person);
            } else if (!person->IsInRetreatment()) {
                // initiate retreatment
                person->InitiateTreatment();
            } else {
                // if retreatment fails, it is a failure and treatment ceases
                this->QuitEngagement(person);
            }
        }
    }
    HCVTreatmentIMPL(datamanagement::ModelData &model_data)
        : TreatmentIMPL(dm) {
        this->allow_retreatment =
            Utils::GetBoolFromConfig("treatment.allow_retreatment", dm);

        this->lost_to_follow_up_probability =
            Utils::GetDoubleFromConfig("treatment.ltfu_probability", dm);
        this->treatment_cost =
            Utils::GetDoubleFromConfig("treatment.treatment_cost", dm);
        this->treatment_utility =
            Utils::GetDoubleFromConfig("treatment.treatment_utility", dm);
        this->retreatment_cost =
            Utils::GetDoubleFromConfig("treatment.retreatment_cost", dm);
        this->treatment_init_probability =
            Utils::GetDoubleFromConfig("treatment.treatment_initiation", dm);
        this->toxicity_cost =
            Utils::GetDoubleFromConfig("treatment.tox_cost", dm);
        this->toxicity_utility =
            Utils::GetDoubleFromConfig("treatment.tox_utility", dm);

        LoadEligibilityData(dm);
        LoadCostData(dm);
        LoadWithdrawalData(dm);
        LoadToxicityData(dm);
        LoadSVRData(dm);
        LoadDurationData(dm);
    }

private:
    // maps to hold treatment data
    using treatmentmap_t =
        std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    treatmentmap_t duration_data;
    treatmentmap_t cost_data;
    treatmentmap_t toxicity_data;
    treatmentmap_t withdrawal_data;

    // user-provided values
    double retreatment_cost;
    treatmentmap_t svr_data;
    double treatment_utility;

    static int callback_treament(void *storage, int count, char **data,
                                 char **columns) {
        Utils::tuple_3i key = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((treatmentmap_t *)storage))[key] = Utils::stod_positive(data[3]);
        return 0;
    }

    std::string TreatmentSQL(std::string column) {
        return "SELECT retreatment, genotype_three, cirrhotic, " + column +
               " FROM treatments;";
    }

    Utils::tuple_3i
    GetTreatmentThruple(std::shared_ptr<person::PersonBase> person) const {
        int geno3 = (person->IsGenotypeThree()) ? 1 : 0;
        int cirr = (person->IsCirrhotic()) ? 1 : 0;
        int retreatment = (int)person->IsInRetreatment();
        return std::make_tuple(retreatment, geno3, cirr);
    }

    void ChargeCostOfCourse(std::shared_ptr<person::PersonBase> person) {
        if (this->cost_data.empty()) {
            spdlog::get("main")->warn("No Treatment Cost Data Found.");
            return;
        }
        double course_cost = cost_data[GetTreatmentThruple(person)];
        ChargeCost(person, course_cost);
        person->SetUtility(treatment_utility, this->UTIL_CATEGORY);
    }

    bool Withdraws(std::shared_ptr<person::PersonBase> person,
                   datamanagement::ModelData &model_data,
                   std::shared_ptr<stats::DeciderBase> decider) {
        if (withdrawal_data.empty()) {
            spdlog::get("main")->warn("No Withdrawal Data Found.");
            return false;
        }

        if (decider->GetDecision(
                {withdrawal_data[GetTreatmentThruple(person)]}) == 0) {
            person->AddWithdrawal();
            this->QuitEngagement(person);
            return true;
        }
        return false;
    }

    void
    CheckIfExperienceToxicity(std::shared_ptr<person::PersonBase> person,
                              std::shared_ptr<stats::DeciderBase> decider) {
        if (toxicity_data.empty()) {
            spdlog::get("main")->warn("No Toxicity Data Found.");
            return;
        }

        if (decider->GetDecision(
                {toxicity_data[GetTreatmentThruple(person)]}) == 1) {
            return;
        }
        person->AddToxicReaction();
        ChargeCost(person, this->toxicity_cost);
        person->SetUtility(this->toxicity_utility, this->UTIL_CATEGORY);
    }

    bool InitiateTreatment(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<stats::DeciderBase> decider) {
        // Do not start treatment until eligible
        if (!IsEligible(person)) {
            return false;
        }
        // person initiates treatment -- set treatment initiation values
        if (decider->GetDecision({treatment_init_probability}) == 0) {
            person->InitiateTreatment();
            return true;
        }
        return false;
    }

    int DecideIfPersonAchievesSVR(std::shared_ptr<person::PersonBase> person,
                                  std::shared_ptr<stats::DeciderBase> decider) {
        if (svr_data.empty()) {
            spdlog::get("main")->warn("No SVR Probability Found!");
            return -1;
        }

        double svr = svr_data[GetTreatmentThruple(person)];
        return decider->GetDecision({svr});
    }

    int LoadCostData(datamanagement::ModelData &model_data) {
        std::string error;
        int rc = dm->SelectCustomCallback(
            TreatmentSQL("cost"), this->callback_treament, &cost_data, error);
        if (rc != 0) {
            spdlog::get("main")->error("Error extracting Treatment Data "
                                       "from treatments! Error Message: {}",
                                       error);
        }
        return rc;
    }

    int LoadWithdrawalData(datamanagement::ModelData &model_data) {
        std::string error;
        int rc = dm->SelectCustomCallback(TreatmentSQL("withdrawal"),
                                          this->callback_treament,
                                          &withdrawal_data, error);

        if (rc != 0) {
            spdlog::get("main")->warn(
                "Error Retrieving Treatment Withdrawal Probability! Error "
                "Message: {}",
                error);
        }
        return rc;
    }

    int LoadToxicityData(datamanagement::ModelData &model_data) {
        std::string error;
        int rc = dm->SelectCustomCallback(TreatmentSQL("toxicity_prob"),
                                          this->callback_treament,
                                          &toxicity_data, error);

        if (rc != 0) {
            spdlog::get("main")->warn(
                "Error Retrieving Treatment Toxicity Probability! Error "
                "Message: {}",
                error);
        }
        return rc;
    }

    int LoadSVRData(datamanagement::ModelData &model_data) {
        std::string error;
        int rc =
            dm->SelectCustomCallback(TreatmentSQL("svr_prob_if_completed"),
                                     this->callback_treament, &svr_data, error);

        if (rc != 0) {
            spdlog::get("main")->warn(
                "Error Retrieving Treatment SVR Probability! Error "
                "Message: {}",
                error);
        }
        return rc;
    }

    int LoadDurationData(datamanagement::ModelData &model_data) {
        std::string error;
        int rc = dm->SelectCustomCallback(TreatmentSQL("duration"),
                                          this->callback_treament,
                                          &duration_data, error);

        if (rc != 0) {
            spdlog::get("main")->warn(
                "Error Retrieving Treatment Duration! Error "
                "Message: {}",
                error);
        }
        return rc;
    }

    void LoadEligibilityData(datamanagement::ModelData &model_data) {
        this->ineligible_behaviors =
            LoadEligibilityVectors("eligibility.ineligible_drug_use", dm);
        this->ineligible_fibrosis = LoadEligibilityVectors(
            "eligibility.ineligible_fibrosis_stages", dm);
        this->ineligible_pregnancy = LoadEligibilityVectors(
            "eligibility.ineligible_pregnancy_states", dm);
        this->ineligible_time_since_linked = Utils::GetIntFromConfig(
            "eligibility.ineligible_time_since_linked", dm, true);
        this->ineligible_time_since_last_use = Utils::GetIntFromConfig(
            "eligibility.ineligible_time_former_threshold", dm, true);
    }

    int GetTreatmentDuration(std::shared_ptr<person::PersonBase> person) {
        if (this->duration_data.empty()) {
            spdlog::get("main")->warn("No Treatment Duration Found!");
            return -1;
        }
        double duration = this->duration_data[GetTreatmentThruple(person)];
        return static_cast<int>(duration);
    }
};

HCVTreatment::HCVTreatment(datamanagement::ModelData &model_data) {
    impl = std::make_unique<HCVTreatmentIMPL>(dm);
}
HCVTreatment::~HCVTreatment() = default;
HCVTreatment::HCVTreatment(HCVTreatment &&) noexcept = default;
HCVTreatment &HCVTreatment::operator=(HCVTreatment &&) noexcept = default;

void HCVTreatment::DoEvent(std::shared_ptr<person::PersonBase> person,
                           datamanagement::ModelData &model_data,
                           std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event

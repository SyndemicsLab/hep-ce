////////////////////////////////////////////////////////////////////////////////
// File: HCVTreatment.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-21                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "HCVTreatment.hpp"
#include "Decider.hpp"
#include "Treatment.hpp"

namespace event {
class HCVTreatmentIMPL : public TreatmentIMPL {
private:
    // user-provided values
    double retreatment_cost;
    bool allow_retreatment = true;
    treatmentmap_t svr_data;

    static int callback_treament(void *storage, int count, char **data,
                                 char **columns) {
        Utils::tuple_3i key = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((treatmentmap_t *)storage))[key] = Utils::stod_positive(data[3]);
        return 0;
    }

    static int GetDataKey(std::shared_ptr<person::PersonBase> person,
                          void *storage) {
        int geno3 = (person->IsGenotypeThree()) ? 1 : 0;
        int cirr = (person->IsCirrhotic()) ? 1 : 0;
        int retreatment = (int)person->IsInRetreatment();
        (*((Utils::tuple_3i *)storage)) =
            std::make_tuple(geno3, cirr, retreatment);
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

    bool isEligible(std::shared_ptr<person::PersonBase> person) const {
        // If currently on a treatment, a new eligibility check should
        // verify retreatment is allowed
        bool check_retreatment = person->HasInitiatedTreatment();
        if (!allow_retreatment && check_retreatment) {
            return false;
        }
        person::FibrosisState fibrosisState = person->GetTrueFibrosisState();
        person::Behavior behavior = person->GetBehavior();
        int timeSinceLastUse = person->GetTimeBehaviorChange();
        int timeSinceLinked = person->GetTimeSinceLinkChange();
        person::PregnancyState pregnancyState = person->GetPregnancyState();
        // if a person is an eligible fibrosis state, behavior, linked time,
        // and hasn't used
        if (isEligibleFibrosisStage(fibrosisState) &&
            isEligibleBehavior(behavior) &&
            (pregnancyState == person::PregnancyState::NA ||
             isEligiblePregnancy(pregnancyState)) &&
            (timeSinceLastUse > ineligible_time_since_last_use) &&
            (timeSinceLinked > ineligible_time_since_linked)) {
            return true;
        }
        return false;
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
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
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

    void CheckIfExperienceToxicity(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
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
        person->SetUtility(toxicity_utility, this->UTIL_CATEGORY);
    }

    bool InitiateTreatment(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
        // Do not start treatment until eligible
        if (!isEligible(person)) {
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

    int LoadCostData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

    int
    LoadWithdrawalData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

    int LoadToxicityData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

    int LoadSVRData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

    int LoadDurationData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

    void
    LoadEligibilityData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // 0. Verify the person is linked before starting treatment
        if (person->GetLinkState() != person::LinkageState::LINKED) {
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
        if (!person->HasInitiatedTreatment()) {
            if (!InitiateTreatment(person, dm, decider)) {
                return;
            }
        }

        // 4. Charge the person for the Course they are on
        ChargeCostOfCourse(person);

        // 5. Check if the person experiences toxicity
        CheckIfExperienceToxicity(person, dm, decider);

        // 6. Determine if the person withdraws from the treatment
        if (Withdraws(person, dm, decider)) {
            return;
        }

        // 7. Determine if the person has been treated long enough, if they
        // achieve SVR
        int duration = GetTreatmentDuration(person, this->GetDataKey);
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
    HCVTreatmentIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm)
        : TreatmentIMPL(dm) {
        this->allow_retreatment =
            Utils::GetBoolFromConfig("treatment.allow_retreatment", dm);

        this->lost_to_follow_up_probability =
            Utils::GetDoubleFromConfig("treatment.ltfu_probability", dm);
        this->treatment_cost =
            Utils::GetDoubleFromConfig("treatment.treatment_cost", dm);
        this->retreatment_cost =
            Utils::GetDoubleFromConfig("treatment.retreatment_cost", dm);
        this->treatment_utility =
            Utils::GetDoubleFromConfig("treatment.treatment_utility", dm);
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
};

HCVTreatment::HCVTreatment(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<HCVTreatmentIMPL>(dm);
}
HCVTreatment::~HCVTreatment() = default;
HCVTreatment::HCVTreatment(HCVTreatment &&) noexcept = default;
HCVTreatment &HCVTreatment::operator=(HCVTreatment &&) noexcept = default;

void HCVTreatment::DoEvent(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event

//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the Treatment Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Treatment.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Treatment::TreatmentIMPL {
    private:
        double discount = 0.0;
        double lost_to_follow_up_probability;
        double treatment_cost;
        double retreatment_cost;
        double treatment_utility;
        double toxicity_cost;
        double toxicity_utility;
        double treatment_init_probability;

        std::vector<std::string> ineligible_behaviors = {};
        std::vector<std::string> ineligible_fibrosis = {};
        int ineligible_time_since_linked = -2;
        int ineligible_time_since_last_use = -2;

        typedef std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                                   Utils::key_equal_2i>
            treatmentmap_t;
        treatmentmap_t duration_data;
        treatmentmap_t cost_data;
        treatmentmap_t svr_data;
        treatmentmap_t toxicity_data;
        treatmentmap_t withdrawal_data;

        void trim(std::string &str) {
            while (str[0] == ' ')
                str.erase(str.begin());
            while (str[str.size() - 1] == ' ')
                str.pop_back();
        }

        static int callback_treament(void *storage, int count, char **data,
                                     char **columns) {
            Utils::tuple_2i key =
                std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
            (*((treatmentmap_t *)storage))[key] = std::stod(data[2]);
            return 0;
        }

        std::string TreatmentSQL(std::string column) {
            return "SELECT genotype_three, cirrhotic, " + column +
                   " FROM treatments;";
        }

        Utils::tuple_2i
        GetTuple(std::shared_ptr<person::PersonBase> person) const {
            int geno3 = (person->IsGenotypeThree()) ? 1 : 0;
            int cirr = (person->IsCirrhotic()) ? 1 : 0;
            return std::make_tuple(geno3, cirr);
        }

        bool isEligible(std::shared_ptr<person::PersonBase> person) const {
            person::FibrosisState fibrosisState =
                person->GetTrueFibrosisState();
            person::Behavior behavior = person->GetBehavior();
            int timeSinceLastUse = person->GetTimeBehaviorChange();
            int timeSinceLinked = person->GetTimeSinceLinkChange();
            person::PregnancyState pregnancyState = person->GetPregnancyState();
            // if a person is an eligible fibrosis state, behavior, linked time,
            // and hasn't used
            if (isEligibleFibrosisStage(fibrosisState) &&
                isEligibleBehavior(behavior) &&
                (timeSinceLastUse > ineligible_time_since_last_use) &&
                (timeSinceLinked > ineligible_time_since_linked)) {
                return true;
            }
            return false;
        }

        bool
        isEligibleFibrosisStage(person::FibrosisState fibrosisState) const {
            for (std::string state : ineligible_fibrosis) {
                person::FibrosisState temp;
                temp << state;
                if (fibrosisState == temp) {
                    return false;
                }
            }
            return true;
        }

        bool isEligibleBehavior(person::Behavior behavior) const {
            for (std::string state : ineligible_behaviors) {
                person::Behavior temp;
                temp << state;
                if (behavior == temp) {
                    return false;
                }
            }
            return true;
        }

        void
        addTreatmentCostAndUtility(std::shared_ptr<person::PersonBase> person,
                                   double cost, double util) {

            double discountAdjustedCost = Event::DiscountEventCost(
                cost, discount, person->GetCurrentTimestep());
            person->AddCost(cost, discountAdjustedCost,
                            cost::CostCategory::TREATMENT);

            person->SetUtility(util);
        }

        bool LostToFollowUp(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
            // If the person is already on treatment, they can't be lost to
            // follow up
            if (person->HasInitiatedTreatment()) {
                return false;
            }
            if (decider->GetDecision({lost_to_follow_up_probability}) == 0) {
                this->quitEngagement(person);
                return true;
            }
            return false;
        }

        void
        ChargeCostOfVisit(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm) {
            double discountAdjustedCost = Event::DiscountEventCost(
                treatment_cost, discount, person->GetCurrentTimestep());
            person->AddCost(treatment_cost, discountAdjustedCost,
                            cost::CostCategory::TREATMENT);
        }

        void ChargeCostOfCourse(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            if (cost_data.empty()) {
                spdlog::get("main")->warn("No Treatment Cost Data Found.");
                return;
            }
            double discountAdjustedCost =
                Event::DiscountEventCost(cost_data[GetTuple(person)], discount,
                                         person->GetCurrentTimestep());
            person->AddCost(cost_data[GetTuple(person)], discountAdjustedCost,
                            cost::CostCategory::TREATMENT);
            person->SetUtility(treatment_utility);
        }

        bool Withdraws(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::shared_ptr<stats::DeciderBase> decider) {
            if (withdrawal_data.empty()) {
                spdlog::get("main")->warn("No Withdrawal Data Found.");
                return false;
            }

            if (decider->GetDecision({withdrawal_data[GetTuple(person)]}) ==
                0) {
                person->AddWithdrawal();
                this->quitEngagement(person);
                return true;
            }
            return false;
        }

        bool
        ExperienceToxicity(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
            if (toxicity_data.empty()) {
                spdlog::get("main")->warn("No Toxicity Data Found.");
                return false;
            }

            if (decider->GetDecision({toxicity_data[GetTuple(person)]}) == 1) {
                return false;
            }
            person->AddToxicReaction();
            this->quitEngagement(person);

            double discountAdjustedCost = Event::DiscountEventCost(
                toxicity_cost, discount, person->GetCurrentTimestep());
            person->AddCost(toxicity_cost, discountAdjustedCost,
                            cost::CostCategory::TREATMENT);
            person->SetUtility(toxicity_utility);
            return true;
        }

        bool
        InitiateTreatment(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm,
                          std::shared_ptr<stats::DeciderBase> decider) {
            if (person->HasInitiatedTreatment()) {
                return true;
            }
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

        void quitEngagement(std::shared_ptr<person::PersonBase> person) {
            // unlink from care
            person->Unlink();
            // reset utility
            person->SetUtility(1.0);
        }

        double ParseDoublesFromConfig(
            std::string configKey,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string data;
            dm->GetFromConfig(configKey, data);
            if (data.empty()) {
                spdlog::get("main")->warn("No {} Found!", configKey);
                data = "0.0";
            }
            return std::stod(data);
        }

        void DecideIfPersonAchievesSVR(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            std::shared_ptr<stats::DeciderBase> decider) {
            if (svr_data.empty()) {
                spdlog::get("main")->warn("No SVR Probability Found!");
                return;
            } else if (duration_data.empty()) {
                spdlog::get("main")->warn("No Treatment Duration Found!");
                return;
            }

            double svr = svr_data[GetTuple(person)];
            double duration = duration_data[GetTuple(person)];

            if (decider->GetDecision({svr}) == 0) {
                person->AddSVR();
                person->ClearHCV();
            }
            if (person->GetTimeSinceTreatmentInitiation() >= (int)duration) {
                person->AddCompletedTreatment();
                this->quitEngagement(person);
            }
        }

        int LoadCostData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(TreatmentSQL("cost"),
                                              this->callback_treament,
                                              &cost_data, error);
            if (rc != 0) {
                spdlog::get("main")->error("Error extracting Treatment Data "
                                           "from treatments! Error Message: {}",
                                           error);
            }
            return rc;
        }

        int LoadWithdrawalData(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

        int
        LoadToxicityData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(TreatmentSQL("toxicity"),
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
            int rc = dm->SelectCustomCallback(
                TreatmentSQL("svr"), this->callback_treament, &svr_data, error);

            if (rc != 0) {
                spdlog::get("main")->warn(
                    "Error Retrieving Treatment SVR Probability! Error "
                    "Message: {}",
                    error);
            }
            return rc;
        }

        int
        LoadDurationData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
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

        int
        LoadEligibilityVectors(std::string data,
                               std::vector<std::string> &ineligibility_vec) {
            if (data.empty()) {
                return 0;
            }
            std::stringstream s(data);
            while (s.good()) {
                std::string substr;
                getline(s, substr, ',');
                trim(substr);
                ineligibility_vec.push_back(substr);
            }
            return 0;
        }

        int LoadEligibilityData(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string eligibility_data;
            dm->GetFromConfig("eligibility.ineligible_drug_use",
                              eligibility_data);
            LoadEligibilityVectors(eligibility_data, ineligible_behaviors);

            eligibility_data.clear();
            dm->GetFromConfig("eligibility.ineligible_fibrosis_stages",
                              eligibility_data);
            LoadEligibilityVectors(eligibility_data, ineligible_fibrosis);

            std::string data;
            dm->GetFromConfig("eligibility.ineligible_time_since_linked", data);
            ineligible_time_since_linked =
                (data.empty()) ? -2 : std::stoi(data);

            data.clear();
            dm->GetFromConfig("eligibility.ineligible_time_former_threshold",
                              data);
            ineligible_time_since_last_use =
                (data.empty()) ? -2 : std::stoi(data);
            return 0;
        }

    public:
        void DoEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            // 1. Check if the Person is Lost To Follow Up (LTFU)
            if (LostToFollowUp(person, dm, decider)) {
                return;
            }

            // 2. Charge the Cost of the Visit (varies if this is retreatment)
            ChargeCostOfVisit(person, dm);

            // 3. Attempt to start treatment, if already on treatment nothing
            // happens
            InitiateTreatment(person, dm, decider);

            // If the person is not on a treatment, exit
            if (!person->HasInitiatedTreatment()) {
                return;
            }

            // 5. Charge the person for the Course they are on
            ChargeCostOfCourse(person, dm);

            // 6. Determine if the person withdraws from the treatment
            if (Withdraws(person, dm, decider)) {
                return;
            }

            // 7. Determine if the person has a toxic reaction
            if (ExperienceToxicity(person, dm, decider)) {
                return;
            }

            // 8. Determine if the person has been treated long enough, if they
            // achieve SVR
            DecideIfPersonAchievesSVR(person, dm, decider);
        }
        TreatmentIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string discount_data;
            int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
            if (!discount_data.empty()) {
                this->discount = std::stod(discount_data);
            }
            lost_to_follow_up_probability =
                ParseDoublesFromConfig("treatment.ltfu_probability", dm);
            treatment_cost =
                ParseDoublesFromConfig("treatment.treatment_cost", dm);
            retreatment_cost =
                ParseDoublesFromConfig("treatment.retreatment_cost", dm);
            treatment_utility =
                ParseDoublesFromConfig("treatment.treatment_utility", dm);
            treatment_init_probability = ParseDoublesFromConfig(
                "treatment.treatment_initialization", dm);
            toxicity_cost = ParseDoublesFromConfig("treatment.tox_cost", dm);
            toxicity_utility =
                ParseDoublesFromConfig("treatment.tox_utility", dm);

            rc = LoadEligibilityData(dm);

            rc = LoadCostData(dm);
            rc = LoadWithdrawalData(dm);
            rc = LoadToxicityData(dm);
            rc = LoadSVRData(dm);
            rc = LoadDurationData(dm);
        }
    };
    Treatment::Treatment(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<TreatmentIMPL>(dm);
    }

    Treatment::~Treatment() = default;
    Treatment::Treatment(Treatment &&) noexcept = default;
    Treatment &Treatment::operator=(Treatment &&) noexcept = default;

    void Treatment::DoEvent(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
        impl->DoEvent(person, dm, decider);
    }

} // namespace event

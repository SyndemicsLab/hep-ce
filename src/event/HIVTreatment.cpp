////////////////////////////////////////////////////////////////////////////////
// File: HIVTreatment.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-17                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-21                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVTreatment.hpp"
#include "Treatment.hpp"

namespace event {
class HIVTreatmentIMPL : public TreatmentIMPL {
private:
    // user-provided values
    // months in treatment required to achieve suppression
    int suppression_months;

    bool InitiateTreatment(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
        return false;
    }

    bool Withdraws(std::shared_ptr<person::PersonBase> person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   std::shared_ptr<stats::DeciderBase> decider) {
        return false;
    }

    void CheckIfExperienceToxicity(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
        std::shared_ptr<stats::DeciderBase> decider) {
        return;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
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
        if (!person->HasInitiatedTreatment(INF_TYPE)) {
            if (!InitiateTreatment(person, dm, decider)) {
                return;
            }
        }

        // Check if treatment causes toxicity
        CheckIfExperienceToxicity(person, dm, decider);

        // Determine if Person withdraws from care
        if (Withdraws(person, dm, decider)) {
            return;
        }
    }

    HIVTreatmentIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm)
        : TreatmentIMPL(dm) {
        this->INF_TYPE = person::InfectionType::HIV;
        this->COST_CATEGORY = cost::CostCategory::HIV;
        this->UTIL_CATEGORY = utility::UtilityCategory::HIV;

        // user input loading
        // defines the number of months necessary to achieve hiv suppression
        this->suppression_months =
            Utils::GetIntFromConfig("hiv_treatment.months_to_suppression", dm);
        this->lost_to_follow_up_probability =
            Utils::GetDoubleFromConfig("hiv_treatment.ltfu_probability", dm);
        this->treatment_cost =
            Utils::GetDoubleFromConfig("hiv_treatment.treatment_cost", dm);
        this->treatment_utility =
            Utils::GetDoubleFromConfig("hiv_treatment.treatment_utility", dm);
        this->treatment_init_probability = Utils::GetDoubleFromConfig(
            "hiv_treatment.treatment_initiation", dm);
        this->toxicity_cost =
            Utils::GetDoubleFromConfig("hiv_treatment.tox_cost", dm);
        this->toxicity_utility =
            Utils::GetDoubleFromConfig("hiv_treatment.tox_utility", dm);
    }
};

HIVTreatment::HIVTreatment(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<HIVTreatmentIMPL>(dm);
}
HIVTreatment::~HIVTreatment() = default;
HIVTreatment::HIVTreatment(HIVTreatment &&) noexcept = default;
HIVTreatment &HIVTreatment::operator=(HIVTreatment &&) noexcept = default;

void HIVTreatment::DoEvent(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event

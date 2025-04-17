////////////////////////////////////////////////////////////////////////////////
// File: HIVTreatment.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-17                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVTreatment.hpp"
#include "Treatment.hpp"

namespace event {
class HIVTreatmentIMPL : public TreatmentIMPL {
private:
    // constants
    utility::UtilityCategory UTIL_CATEGORY = utility::UtilityCategory::HIV;
    person::InfectionType INF_TYPE = person::InfectionType::HIV;

    // user-provided values
    int suppression_months;

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        if (person->GetLinkState(INF_TYPE) != person::LinkageState::LINKED) {
            return;
        }
    }

    HIVTreatmentIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm)
        : TreatmentIMPL(dm) {
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
        this->suppression_months =
            Utils::GetIntFromConfig("hiv_treatment.months_to_suppression", dm);
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

////////////////////////////////////////////////////////////////////////////////
// File: treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hcv/treatment.hpp>

#include "hcv/internals/treatment_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace hcv {
TreatmentImpl::TreatmentImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name)
    : TreatmentBase(dm, log_name) {}

int TreatmentImpl::Execute(model::Person &person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           model::Sampler &sampler) {
    // 0. Verify the person is linked before starting treatment
    if (person.GetLinkState() != data::LinkageState::LINKED) {
        return;
    }

    // 1. Check if the Person is Lost To Follow Up (LTFU)
    if (LostToFollowUp(person, sampler)) {
        return;
    }

    // 2. Charge the Cost of the Visit (varies if this is retreatment)
    int num_treatments =
        person.GetWithdrawals() + person.GetCompletedTreatments();
    double visit_cost = (num_treatments > 0) ? GetTreatmentCosts().retreatment
                                             : GetTreatmentCosts().treatment;
    ChargeCost(person, visit_cost);

    // 3. Attempt to start primary treatment, if already on treatment
    // nothing happens
    if (!person.HasInitiatedTreatment() &&
        !InitiateTreatment(person, sampler)) {
        return;
    }

    // 4. Charge the person for the Course they are on
    ChargeCostOfCourse(person);

    // 5. Check if the person experiences toxicity
    CheckIfExperienceToxicity(person, sampler);

    // 6. Determine if the person withdraws from the treatment
    if (Withdraws(person, dm, sampler)) {
        return;
    }

    // 7. Determine if the person has been treated long enough and, if so,
    // if they achieve SVR
    int duration = GetTreatmentDuration(person);
    if (person.GetTimeSinceTreatmentInitiation() == duration) {
        person.AddCompletedTreatment();
        int decision = DecideIfPersonAchievesSVR(person, sampler);
        if (decision == 0) {
            person.AddSVR();
            person.ClearHCV();
            person.ClearDiagnosis();
            QuitEngagement(person);
        } else if (!person.IsInRetreatment()) {
            // initiate retreatment
            person.InitiateTreatment();
        } else {
            // if retreatment fails, it is a failure and treatment ceases
            QuitEngagement(person);
        }
    }
}

} // namespace hcv
} // namespace event
} // namespace hepce
////////////////////////////////////////////////////////////////////////////////
// File: treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Mo Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hiv/treatment.hpp>

#include "internals/treatment_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace hiv {
TreatmentImpl::TreatmentImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name = "console")
    : TreatmentBase(model_data, log_name) {

    // load treatment course data - course defined in sim.conf
    _course_name =
        utils::GetStringFromConfig("hiv_treatment.course", model_data);
    LoadCourseData(model_data);
    LoadUtilityData(model_data);
}

int TreatmentImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // Ensure that Person is linked to care
    if (person.GetLinkState(data::InfectionType::HIV) !=
        data::LinkageState::LINKED) {
        return;
    }

    // Determine if Person is lost to follow up before beginning treatment
    if (LostToFollowUp(person, sampler)) {
        return;
    }

    // Charge the cost of the visit
    ChargeCost(person, GetTreatmentCosts().treatment);

    // Determine if Person initiates treatment
    if (!person.HasInitiatedTreatment() &&
        !InitiateTreatment(person, sampler)) {
        return;
    }

    // Charge the cost of the treatment
    ChargeCostOfCourse(person);

    // Check if treatment causes toxicity
    CheckIfExperienceToxicity(person, sampler);

    // Determine if Person withdraws from care
    if (Withdraws(person, sampler)) {
        // if achieving suppression through treatment, it instantaneously
        // ends
        this->LoseSuppression(person);
    }

    // apply suppression if the person has been in treatment long enough
    // must equal suppression months so that this is only triggered at the
    // time of having been in treatment long enough
    if (person.GetTimeSinceTreatmentInitiation() ==
        _treatment_sql_data[_course_name].suppression_months) {
        ApplySuppression(person);
    }

    // if person is has a low CD4/T-cell count and has been on treatment
    // long enough, restore their CD4 count to high
    if (IsLowCD4(person) &&
        person.GetTimeSinceTreatmentInitiation() ==
            _treatment_sql_data[_course_name].restore_high_cd4_months) {
        this->RestoreHighCD4(person);
    }
}
} // namespace hiv
} // namespace event
} // namespace hepce
////////////////////////////////////////////////////////////////////////////////
// File: treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-21                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/hiv/treatment.hpp>

// Library Includes
#include <hepce/utils/config.hpp>

// Local Includes
#include "internals/treatment_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
// Factory
std::unique_ptr<hepce::event::Event>
Treatment::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<TreatmentImpl>(model_data, log_name);
}

// Constructor
TreatmentImpl::TreatmentImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : TreatmentBase(model_data, log_name) {

    // load treatment course data - course defined in sim.conf
    _course_name =
        utils::GetStringFromConfig("hiv_treatment.course", model_data);
    LoadCourseData(model_data);
    LoadUtilityData(model_data);
}

// Execute
int TreatmentImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // Ensure that Person is linked to care
    if (person.GetLinkageDetails(GetInfectionType()).link_state !=
        data::LinkageState::kLinked) {
        return 0;
    }

    // Determine if Person is lost to follow up before beginning treatment
    if (LostToFollowUp(person, sampler)) {
        return 0;
    }

    // Charge the cost of the visit
    ChargeCost(person, GetTreatmentCosts().treatment);

    // Determine if Person initiates treatment
    if (!person.GetTreatmentDetails(GetInfectionType()).initiated_treatment &&
        !InitiateTreatment(person, sampler)) {
        return 0;
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

    int time_since_init = person.GetCurrentTimestep() -
                          person.GetTreatmentDetails(GetInfectionType())
                              .time_of_treatment_initiation;
    // apply suppression if the person has been in treatment long enough
    // must equal suppression months so that this is only triggered at the
    // time of having been in treatment long enough
    if (time_since_init ==
        _treatment_sql_data[_course_name].suppression_months) {
        ApplySuppression(person);
    }

    // if person is has a low CD4/T-cell count and has been on treatment
    // long enough, restore their CD4 count to high
    if (IsLowCD4(person) &&
        time_since_init ==
            _treatment_sql_data[_course_name].restore_high_cd4_months) {
        RestoreHighCD4(person);
    }
}

// Private Methods
bool TreatmentImpl::InitiateTreatment(model::Person &person,
                                      model::Sampler &sampler) {
    // do not start treatment if the individual is not eligible
    if (!IsEligible(person)) {
        return false;
    }
    if (sampler.GetDecision({GetTreatmentProbabilities().initialization}) ==
        0) {
        person.InitiateTreatment(GetInfectionType());
        return true;
    }
    return false;
}

void TreatmentImpl::ResetUtility(model::Person &person) const {
    utils::tuple_2i key;
    switch (person.GetHIVDetails().hiv) {
    case data::HIV::kHiSu:
    case data::HIV::kHiUn:
        key = std::make_tuple(0, 1);
        person.SetUtility(_utility_data.at(key), GetEventUtilityCategory());
        break;
    case data::HIV::kLoSu:
    case data::HIV::kLoUn:
        key = std::make_tuple(0, 0);
        person.SetUtility(_utility_data.at(key), GetEventUtilityCategory());
        break;
    default:
        break;
    }
}

bool TreatmentImpl::Withdraws(model::Person &person, model::Sampler &sampler) {
    if (_treatment_sql_data[_course_name].withdrawal_prob == 0) {
        // spdlog::get("main")->warn(
        //     "HIV treatment withdrawal probability is "
        //     "0. If this isn't intended, check your inputs!");
    }

    if (sampler.GetDecision(
            {_treatment_sql_data[_course_name].withdrawal_prob}) == 0) {
        person.AddWithdrawal(GetInfectionType());
        QuitEngagement(person);
        return true;
    }
    return false;
}

void TreatmentImpl::CheckIfExperienceToxicity(model::Person &person,
                                              model::Sampler &sampler) {
    if (sampler.GetDecision(
            {_treatment_sql_data[_course_name].toxicity_prob}) == 1) {
        return;
    }
    person.AddToxicReaction(GetInfectionType());
    ChargeCost(person, GetTreatmentCosts().toxicity);
    SetEventUtility(GetTreatmentUtilities().toxicity);
    AddEventUtility(person);
}

/// @brief Used to set person's HIV utility after engaging with treatment
/// @param
void TreatmentImpl::SetTreatmentUtility(model::Person &person) {
    utils::tuple_2i key;
    switch (person.GetHIVDetails().hiv) {
    case data::HIV::kHiSu:
    case data::HIV::kHiUn:
        key = std::make_tuple(1, 1);
        SetEventUtility(_utility_data[key]);
        AddEventUtility(person);
        break;
    case data::HIV::kLoSu:
    case data::HIV::kLoUn:
        key = std::make_tuple(1, 0);
        SetEventUtility(_utility_data[key]);
        AddEventUtility(person);
        break;
    default:
        break;
    }
}

/// @brief Used to reset person's HIV utility after discontinuing treatment
/// @param
void TreatmentImpl::ResetUtility(model::Person &person) {
    utils::tuple_2i key;
    switch (person.GetHIVDetails().hiv) {
    case data::HIV::kHiSu:
    case data::HIV::kHiUn:
        key = std::make_tuple(0, 1);
        SetEventUtility(_utility_data[key]);
        AddEventUtility(person);
        break;
    case data::HIV::kLoSu:
    case data::HIV::kLoUn:
        key = std::make_tuple(0, 0);
        SetEventUtility(_utility_data[key]);
        AddEventUtility(person);
        break;
    default:
        break;
    }
}

void TreatmentImpl::ApplySuppression(model::Person &person) {
    switch (person.GetHIVDetails().hiv) {
    case data::HIV::kHiUn:
        person.SetHIV(data::HIV::kHiSu);
        break;
    case data::HIV::kLoUn:
        person.SetHIV(data::HIV::kLoSu);
        break;
    default:
        break;
    }
}

void TreatmentImpl::LoseSuppression(model::Person &person) {
    switch (person.GetHIVDetails().hiv) {
    case data::HIV::kHiSu:
        person.SetHIV(data::HIV::kHiUn);
        break;
    case data::HIV::kLoSu:
        person.SetHIV(data::HIV::kLoUn);
        break;
    default:
        break;
    }
}

} // namespace hiv
} // namespace event
} // namespace hepce
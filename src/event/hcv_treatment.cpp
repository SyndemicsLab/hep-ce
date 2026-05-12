////////////////////////////////////////////////////////////////////////////////
// File: hcv_treatment.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-13                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/hcv_treatment_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> HCVTreatment::Create(const data::Inputs &inputs,
                                            const std::string &log_name) {
    return std::make_unique<HCVTreatment>(inputs, log_name);
}

// Execute
void HCVTreatment::Execute(model::Person &person,
                           const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // 0. Verify the person is linked before starting treatment
    if (person.GetLinkageDetails(GetInfectionType()).link_state !=
        data::LinkageState::kLinked) {
        return;
    }

    double visit_cost =
        (person.GetTreatmentDetails(GetInfectionType()).in_salvage_treatment)
            ? GetTreatmentCosts().salvage
            : GetTreatmentCosts().treatment;

    if (!person.GetTreatmentDetails(GetInfectionType()).initiated_treatment) {
        // 1. Check if the Person is Lost To Follow Up (LTFU)
        if (LostToFollowUp(person, sampler)) {
            return;
        }
        // 2. Charge the Cost of the Visit (varies if this is salvage)
        AddEventCost(person, visit_cost);

        // 3. Attempt to start primary treatment, if already on treatment
        // nothing happens
        if (!InitiateTreatment(person, sampler)) {
            return;
        }
    } else {
        // 2. Charge the Cost of the Visit (varies if this is salvage)
        AddEventCost(person, visit_cost);
    }

    // 4. Charge the person for the treatment course they are on
    AddEventCost(person, _treatment_sql_data[GetTreatmentThruple(person)].cost);
    AddEventUtility(person, GetTreatmentUtilities().treatment);

    // 5. Check if the person experiences toxicity
    CheckIfExperienceToxicity(person, sampler);

    // 6. Determine if the person withdraws from the treatment
    if (Withdraws(person, sampler)) {
        return;
    }

    // 7. Determine if the person has been treated long enough and, if so,
    // if they achieve SVR
    int duration = GetTreatmentDuration(person);
    int time_since_starting_treatment =
        GetTimeSince(person, person.GetTreatmentDetails(GetInfectionType())
                                 .time_of_treatment_initiation);
    if (time_since_starting_treatment == duration) {
        person.AddCompletedTreatment(GetInfectionType());
        int decision = DecideIfPersonAchievesSVR(person, sampler);
        if (decision == 0) {
            // person achieves SVR and clears of infection, ceases treatment
            // because cured
            person.AddSVR();
            person.ClearHCV(false);
            person.ClearDiagnosis(GetInfectionType());
            QuitEngagement(person);
        } else if (!person.GetTreatmentDetails(GetInfectionType())
                        .in_salvage_treatment) {
            // initiate salvage
            person.InitiateTreatment(GetInfectionType());
        } else {
            // if salvage fails, it is a failure and treatment ceases
            QuitEngagement(person);
        }
    }
}

void HCVTreatment::LoadData() {
    SetUtilityCategory(model::UtilityCategory::kTreatment);
    SetCostCategory(model::CostCategory::kTreatment);
    std::any storage = hcvtreatmentmap_t{};
    try {
        GetInputs().SelectFromDatabase(TreatmentSQL(), Callback, storage, {});
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting HCV Treatment Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _treatment_sql_data = std::any_cast<hcvtreatmentmap_t>(storage);
    if (_treatment_sql_data.empty()) {
        hepce::utils::LogWarning(GetLogName(), "Treatment Table is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }

    storage = hcvtreatmentinitmap_t{};
    try {
        GetInputs().SelectFromDatabase(TreatmentInitializationSQL(),
                                       CallbackTreatmentInitialization, storage,
                                       {});
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting HCV Treatment Initialization Data: " << e.what();
        hepce::utils::LogError(GetLogName(), msg.str());
        return;
    }
    _treatment_init_sql_data = std::any_cast<hcvtreatmentinitmap_t>(storage);
    if (_treatment_init_sql_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Treatment Initiation Table is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

bool HCVTreatment::InitiateTreatment(model::Person &person,
                                     const model::Sampler &sampler) const {
    double treatment_initiation = 0.0;
    try {
        treatment_initiation = _treatment_init_sql_data.at(
            static_cast<int>(person.GetPregnancyDetails().pregnancy_state));
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Warning - Failed Reading Treatment Initialization Data: "
            << e.what() << std::endl;
        msg << "Setting treatment initiation to 0.0...";
        hepce::utils::LogWarning(GetLogName(), msg.str());
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
    if (IsEligible(person) &&
        (sampler.GetDecision({treatment_initiation}) == 0)) {
        person.InitiateTreatment(GetInfectionType());
        return true;
    }
    return false;
}

} // namespace event
} // namespace hepce

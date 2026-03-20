////////////////////////////////////////////////////////////////////////////////
// File: death.cpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/utils/logging.hpp>

#include "internals/death_internals.hpp"

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> Death::Create(const data::Inputs &inputs,
                                     const std::string &log_name) {
    return std::make_unique<Death>(inputs, log_name);
}

// Execute
void Death::Execute(model::Person &person, const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    if (check_overdose && FatalOverdose(person, sampler)) {
        return;
    }
    if (check_hiv && HivDeath(person, sampler)) {
        return;
    }
    if (ReachedMaxAge(person)) {
        return;
    }

    // Calculate background mortality rate based on age, gender, and IDU
    double fib_death_probability = GetFibrosisMortalityProbability(person);

    // 2. Get fatal OD probability.
    double bg_death_probability = 0.0;
    double smr = 0.0;
    GetSMRandBackgroundProb(person, bg_death_probability, smr);

    // 3. Decide whether the person dies. If not, unset their overdose
    // property.
    double bg_mortality_probability = utils::RateToProbability(
        utils::ProbabilityToRate(bg_death_probability) * smr);
    double total_death_probability =
        bg_mortality_probability + fib_death_probability;

    std::vector<double> probabilities = {bg_mortality_probability,
                                         fib_death_probability,
                                         1 - total_death_probability};

    int decision = sampler.GetDecision(probabilities);
    if (decision == 0) {
        Die(person, data::DeathReason::kBackground);
    } else if (decision == 1) {
        Die(person, data::DeathReason::kLiver);
    }
}

void Death::LoadData() {
    if (utils::FindInEventList("overdose", GetInputs())) {
        check_overdose = true;
        _probability_of_overdose_fatality = utils::GetDoubleFromConfig(
            "overdose.probability_of_overdose_fatality", GetInputs());
        _fatal_overdose_cost = utils::GetDoubleFromConfig(
            "overdose.fatal_overdose_cost", GetInputs());
    }
    if (utils::FindInEventList("hivinfection", GetInputs())) {
        check_hiv = true;
        _hiv_mortality_probability =
            utils::GetDoubleFromConfig("mortality.hiv", GetInputs());
    }
    LoadBackgroundMortality();
}

// Private Methods
bool Death::ReachedMaxAge(model::Person &person) const {
    if (person.GetAge() >= 1200) {
        Die(person, data::DeathReason::kAge);
        return true;
    }
    return false;
}

void Death::LoadBackgroundMortality() {
    std::string query = BackgroundMortalitySQL();
    std::any storage = _background_data;

    try {
        GetInputs().SelectFromDatabase(
            query,
            [](std::any &storage, const SQLite::Statement &stmt) {
                backgroundmap_t *temp =
                    std::any_cast<backgroundmap_t>(&storage);
                utils::tuple_3i tup = std::make_tuple(
                    stmt.getColumn(0).getInt(), stmt.getColumn(1).getInt(),
                    stmt.getColumn(2).getInt());
                struct BackgroundSmr bgsmr = {stmt.getColumn(3).getDouble(),
                                              stmt.getColumn(4).getDouble()};
                (*temp)[tup] = bgsmr;
            },
            storage, {});
    } catch (std::exception &e) {
        std::string msg = "SQL Exception getting Background mortality: " +
                          std::string(e.what());
        hepce::utils::LogError(GetLogName(), msg);
    }

    _background_data = std::any_cast<backgroundmap_t>(storage);

    if (_background_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Background Mortality Data is Empty...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
    }
}

bool Death::FatalOverdose(model::Person &person,
                          const model::Sampler &sampler) {
    if (!person.GetCurrentlyOverdosing()) {
        return false;
    }

    if (sampler.GetDecision({_probability_of_overdose_fatality,
                             1 - _probability_of_overdose_fatality}) != 0) {
        person.ToggleOverdose();
        SetCostCategory(model::CostCategory::kOverdose);
        AddEventCost(person, _fatal_overdose_cost);
        return false;
    }
    Die(person, data::DeathReason::kOverdose);
    return true;
}

bool Death::HivDeath(model::Person &person,
                     const model::Sampler &sampler) const {
    if (person.GetHIVDetails().hiv == data::HIV::kNone) {
        return false;
    }
    if (sampler.GetDecision({_hiv_mortality_probability}) == 0) {
        Die(person, data::DeathReason::kHiv);
        return true;
    }
    return false;
}

double Death::GetFibrosisMortalityProbability(model::Person &person) const {

    if (person.GetHCVDetails().fibrosis_state == data::FibrosisState::kF4) {
        if (person.GetHCVDetails().hcv == data::HCV::kNone) {
            return _f4_uninfected_probability;
        } else {
            return _f4_infected_probability;
        }

    } else if (person.GetHCVDetails().fibrosis_state ==
               data::FibrosisState::kDecomp) {
        if (person.GetHCVDetails().hcv == data::HCV::kNone) {
            return _decomp_uninfected_probability;
        } else {
            return _decomp_infected_probability;
        }
    }
    return 0;
}

void Death::GetSMRandBackgroundProb(model::Person &person, double &background,
                                    double &smr) const {

    if (_background_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Invalid Background Death Data Found...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        background = 0;
        smr = 0;
        return;
    }
    // age, gender, drug
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int drug_behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
    auto temp = _background_data.at(tup);
    background = temp.back_mort;
    smr = temp.smr;
}

} // namespace event
} // namespace hepce

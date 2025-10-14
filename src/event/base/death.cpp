////////////////////////////////////////////////////////////////////////////////
// File: death.cpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/base/death.hpp>

#include <hepce/utils/logging.hpp>

#include "internals/death_internals.hpp"

namespace hepce {
namespace event {
namespace base {

// Factory
std::unique_ptr<hepce::event::Event>
Death::Create(datamanagement::ModelData &model_data,
              const std::string &log_name) {
    return std::make_unique<DeathImpl>(model_data, log_name);
}

// Constructor
DeathImpl::DeathImpl(datamanagement::ModelData &model_data,
                     const std::string &log_name)
    : EventBase(model_data, log_name),
      _f4_infected_probability(
          utils::GetDoubleFromConfig("mortality.f4_infected", model_data)),
      _f4_uninfected_probability(
          utils::GetDoubleFromConfig("mortality.f4_uninfected", model_data)),
      _decomp_infected_probability(
          utils::GetDoubleFromConfig("mortality.decomp_infected", model_data)),
      _decomp_uninfected_probability(utils::GetDoubleFromConfig(
          "mortality.decomp_uninfected", model_data)) {
    LoadData(model_data);
}

// Execute
void DeathImpl::Execute(model::Person &person, model::Sampler &sampler) {
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

void DeathImpl::LoadData(datamanagement::ModelData &model_data) {
    if (utils::FindInEventList("overdose", model_data)) {
        check_overdose = true;
        _probability_of_overdose_fatality = utils::GetDoubleFromConfig(
            "mortality.probability_of_overdose_fatality", model_data);
    }
    if (utils::FindInEventList("hivinfection", model_data)) {
        check_hiv = true;
        _hiv_mortality_probability =
            utils::GetDoubleFromConfig("mortality.hiv", model_data);
    }
    LoadBackgroundMortality(model_data);
}

// Private Methods
bool DeathImpl::ReachedMaxAge(model::Person &person) {
    if (person.GetAge() >= 1200) {
        Die(person, data::DeathReason::kAge);
        return true;
    }
    return false;
}

void DeathImpl::LoadBackgroundMortality(datamanagement::ModelData &model_data) {
    std::string query = BackgroundMortalitySQL();
    std::any storage = _background_data;

    try {
        model_data.GetDBSource("inputs").Select(
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
            storage);
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

bool DeathImpl::FatalOverdose(model::Person &person, model::Sampler &sampler) {
    if (!person.GetCurrentlyOverdosing()) {
        return false;
    }

    if (sampler.GetDecision({_probability_of_overdose_fatality,
                             1 - _probability_of_overdose_fatality}) != 0) {
        person.ToggleOverdose();
        return false;
    }
    Die(person, data::DeathReason::kOverdose);
    return true;
}

bool DeathImpl::HivDeath(model::Person &person, model::Sampler &sampler) {
    if (person.GetHIVDetails().hiv == data::HIV::kNone) {
        return false;
    }
    if (sampler.GetDecision({_hiv_mortality_probability}) == 0) {
        Die(person, data::DeathReason::kHiv);
        return true;
    }
    return false;
}

double DeathImpl::GetFibrosisMortalityProbability(model::Person &person) {

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

void DeathImpl::GetSMRandBackgroundProb(model::Person &person,
                                        double &bg_death_probability,
                                        double &smr) {

    if (_background_data.empty()) {
        hepce::utils::LogWarning(GetLogName(),
                                 "Invalid Background Death Data Found...");
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        bg_death_probability = 0;
        smr = 0;
        return;
    }
    // age, gender, drug
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int drug_behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
    bg_death_probability = _background_data[tup].back_mort;
    smr = _background_data[tup].smr;
}

} // namespace base
} // namespace event
} // namespace hepce

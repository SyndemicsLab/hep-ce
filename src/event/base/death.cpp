////////////////////////////////////////////////////////////////////////////////
// File: death.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/base/death.hpp>

#include "internals/death_internals.hpp"

namespace hepce {
namespace event {
namespace base {
bool DeathImpl::ReachedMaxAge(model::Person &person) {
    if (person.GetAge() >= 1200) {
        Die(person, data::DeathReason::AGE);
        return true;
    }
    return false;
}

int DeathImpl::LoadBackgroundMortality(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string query = BackgroundMortalitySQL();
    std::string error;
    int rc = dm->SelectCustomCallback(query, callback_background,
                                      &background_data, error);
    if (rc != 0) {
        spdlog::get("main")->error(
            "Error extracting Death Data from background_mortality and "
            "SMR! Error Message: {}",
            error);
        spdlog::get("main")->info("Query: {}", query);
    }
    if (background_data.empty()) {
        spdlog::get("main")->warn(
            "Setting background death probability to zero. No Data "
            "Found for background_mortality and "
            "SMR with the query: {}",
            query);
    }
    return rc;
}

int DeathImpl::CheckOverdoseTable(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND "
                        "name='overdoses';";
    datamanagement::Table table;
    int rc = dm->Select(query, table);
    if (rc == 0 && table.empty()) {
        spdlog::get("main")->info(
            "No Overdoses table Found During Death. Ignoring.");
        rc = -1;
    }
    return rc;
}

int DeathImpl::LoadOverdoseData(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    int rc = CheckOverdoseTable(dm);
    if (rc != 0) {
        return rc;
    }
    std::string error;
    rc = dm->SelectCustomCallback(OverdoseSQL(), this->callback_overdose,
                                  &overdose_data, error);
    if (rc != 0) {
        spdlog::get("main")->error("Error extracting Fatal Overdose "
                                   "Probability! Error Message: {}",
                                   error);
    }
    return rc;
}

bool DeathImpl::FatalOverdose(
    model::Person &person, std::shared_ptr<datamanagement::DataManagerBase> dm,
    model::Sampler &decider) {
    if (!person.GetCurrentlyOverdosing()) {
        return false;
    }

    if (overdose_data.empty()) {
        spdlog::get("main")->warn("No Fatal Overdose Probability Found!");
        person.ToggleOverdose();
        return false;
    }

    int moud = (int)person.GetMoudState();
    int drug_behavior = (int)person.GetBehavior();
    utils::tuple_2i tup = std::make_tuple(moud, drug_behavior);
    double probability = overdose_data[tup];

    if (decider.GetDecision({probability, 1 - probability}) != 0) {
        person.ToggleOverdose();
        return false;
    }
    Die(person, data::DeathReason::OVERDOSE);
    return true;
}

void DeathImpl::GetFibrosisMortalityProb(
    model::Person &person, std::shared_ptr<datamanagement::DataManagerBase> dm,
    double &prob) {

    if (person.GetTrueFibrosisState() == data::FibrosisState::F4) {
        if (person.GetHCV() == data::HCV::NONE) {
            prob = f4_uninfected_probability;
        } else {
            prob = f4_infected_probability;
        }

    } else if (person.GetTrueFibrosisState() == data::FibrosisState::DECOMP) {
        if (person.GetHCV() == data::HCV::NONE) {
            prob = decomp_uninfected_probability;
        } else {
            prob = decomp_infected_probability;
        }
    } else {
        prob = 0;
    }
}

void DeathImpl::GetSMRandBackgroundProb(
    model::Person &person, std::shared_ptr<datamanagement::DataManagerBase> dm,
    double &backgroundMortProb, double &smr) {

    if (background_data.empty()) {
        spdlog::get("main")->warn(
            "Setting background death probability to zero. No Data "
            "Found for background_mortality and "
            "SMR");
        backgroundMortProb = 0;
        smr = 0;
        return;
    }
    // age, gender, drug
    int age_years = (int)(person.GetAge() / 12.0);
    int gender = (int)person.GetSex();
    int drug_behavior = (int)person.GetBehavior();
    utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
    backgroundMortProb = background_data[tup].back_mort;
    smr = background_data[tup].smr;
}

int DeathImpl::Execute(model::Person &person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       model::Sampler &sampler) {
    if (ReachedMaxAge(person)) {
        return;
    }
    if (FatalOverdose(person, dm, sampler)) {
        return;
    }

    // Calculate background mortality rate based on age, gender, and IDU
    double fibrosisDeathProb = 0.0;
    GetFibrosisMortalityProb(person, dm, fibrosisDeathProb);

    // 2. Get fatal OD probability.
    double backgroundMortProb = 0.0;
    double smr = 0.0;
    GetSMRandBackgroundProb(person, dm, backgroundMortProb, smr);

    // 3. Decide whether the person dies. If not, unset their overdose
    // property.
    double totalProb = (backgroundMortProb * smr) + fibrosisDeathProb;

    std::vector<double> probVec = {(backgroundMortProb * smr),
                                   fibrosisDeathProb, 1 - totalProb};

    int retIdx = sampler.GetDecision(probVec);
    if (retIdx == 0) {
        Die(person, data::DeathReason::BACKGROUND);
    } else if (retIdx == 1) {
        Die(person, data::DeathReason::LIVER);
    }
}

std::unique_ptr<hepce::event::Event>
Death::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
              const std::string &log_name) {
    return std::make_unique<DeathImpl>(dm, log_name);
}
} // namespace base
} // namespace event
} // namespace hepce

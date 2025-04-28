////////////////////////////////////////////////////////////////////////////////
// File: death.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/base/death.hpp>

#include "internals/death_internals.hpp"

namespace hepce {
namespace event {
namespace base {
std::unique_ptr<hepce::event::Event>
Death::Create(datamanagement::ModelData &model_data,
              const std::string &log_name) {
    return std::make_unique<DeathImpl>(model_data, log_name);
}

bool DeathImpl::ReachedMaxAge(model::Person &person) {
    if (person.GetAge() >= 1200) {
        Die(person, data::DeathReason::kAge);
        return true;
    }
    return false;
}

int DeathImpl::LoadBackgroundMortality(datamanagement::ModelData &model_data) {
    std::string query = BackgroundMortalitySQL();
    std::any storage = _background_data;

    model_data.GetDBSource("inputs").Select(
        query,
        [](std::any &storage, const SQLite::Statement &stmt) {
            utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                                  stmt.getColumn(1).getInt(),
                                                  stmt.getColumn(2).getInt());
            struct BackgroundSmr temp = {stmt.getColumn(3).getDouble(),
                                         stmt.getColumn(4).getDouble()};
            std::any_cast<backgroundmap_t>(storage)[tup] = temp;
        },
        storage);

    _background_data = std::any_cast<backgroundmap_t>(storage);

    if (_background_data.empty()) {
        // Warn Empty
    }
    return 0;
}

int DeathImpl::CheckOverdoseTable(datamanagement::ModelData &model_data) {
    std::unordered_map<int, datamanagement::source::BindingVariant> bindings;
    bindings[1] = 'table';
    bindings[2] = 'overdoses';
    std::vector<std::string> names;
    std::any storage = names;
    model_data.GetDBSource("inputs").Select(
        OverdoseTableSQL(),
        [](std::any &storage, const SQLite::Statement &stmt) {
            std::any_cast<std::vector<std::string>>(storage).emplace_back(
                stmt.getColumn(0).getString());
        },
        storage);

    names = std::any_cast<std::vector<std::string>>(storage);
    if (names.empty()) {
        // Warn Empty
        return -1;
    }
    return 0;
}

int DeathImpl::LoadOverdoseData(datamanagement::ModelData &model_data) {
    int rc = CheckOverdoseTable(model_data);
    if (rc != 0) {
        return rc;
    }
    std::any storage = _overdose_data;
    model_data.GetDBSource("inputs").Select(
        OverdoseSQL(),
        [](std::any &storage, const SQLite::Statement &stmt) {
            utils::tuple_2i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                                  stmt.getColumn(1).getInt());
            std::any_cast<overdosemap_t>(storage)[tup] =
                stmt.getColumn(2).getDouble();
        },
        storage);

    _overdose_data = std::any_cast<overdosemap_t>(storage);
    if (_overdose_data.empty()) {
        // Warn Empty
    }
    return 0;
}

bool DeathImpl::FatalOverdose(model::Person &person, model::Sampler &decider) {
    if (!person.GetCurrentlyOverdosing()) {
        return false;
    }

    if (_overdose_data.empty()) {
        spdlog::get("main")->warn("No Fatal Overdose Probability Found!");
        person.ToggleOverdose();
        return false;
    }

    int moud = static_cast<int>(person.GetMoudState());
    int drug_behavior = static_cast<int>(person.GetBehavior());
    utils::tuple_2i tup = std::make_tuple(moud, drug_behavior);
    double probability = _overdose_data[tup];

    if (decider.GetDecision({probability, 1 - probability}) != 0) {
        person.ToggleOverdose();
        return false;
    }
    Die(person, data::DeathReason::kOverdose);
    return true;
}

void DeathImpl::GetFibrosisMortalityProb(model::Person &person, double &prob) {

    if (person.GetTrueFibrosisState() == data::FibrosisState::kF4) {
        if (person.GetHCV() == data::HCV::kNone) {
            prob = _f4_uninfected_probability;
        } else {
            prob = _f4_infected_probability;
        }

    } else if (person.GetTrueFibrosisState() == data::FibrosisState::kDecomp) {
        if (person.GetHCV() == data::HCV::kNone) {
            prob = _decomp_uninfected_probability;
        } else {
            prob = _decomp_infected_probability;
        }
    } else {
        prob = 0;
    }
}

void DeathImpl::GetSMRandBackgroundProb(model::Person &person,
                                        double &backgroundMortProb,
                                        double &smr) {

    if (_background_data.empty()) {
        spdlog::get("main")->warn(
            "Setting background death probability to zero. No Data "
            "Found for background_mortality and "
            "SMR");
        backgroundMortProb = 0;
        smr = 0;
        return;
    }
    // age, gender, drug
    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int drug_behavior = static_cast<int>(person.GetBehavior());
    utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
    backgroundMortProb = _background_data[tup].back_mort;
    smr = _background_data[tup].smr;
}

int DeathImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (ReachedMaxAge(person)) {
        return;
    }
    if (FatalOverdose(person, sampler)) {
        return;
    }

    // Calculate background mortality rate based on age, gender, and IDU
    double fibrosisDeathProb = 0.0;
    GetFibrosisMortalityProb(person, fibrosisDeathProb);

    // 2. Get fatal OD probability.
    double backgroundMortProb = 0.0;
    double smr = 0.0;
    GetSMRandBackgroundProb(person, backgroundMortProb, smr);

    // 3. Decide whether the person dies. If not, unset their overdose
    // property.
    double totalProb = (backgroundMortProb * smr) + fibrosisDeathProb;

    std::vector<double> probVec = {(backgroundMortProb * smr),
                                   fibrosisDeathProb, 1 - totalProb};

    int retIdx = sampler.GetDecision(probVec);
    if (retIdx == 0) {
        Die(person, data::DeathReason::kBackground);
    } else if (retIdx == 1) {
        Die(person, data::DeathReason::kLiver);
    }
}

} // namespace base
} // namespace event
} // namespace hepce

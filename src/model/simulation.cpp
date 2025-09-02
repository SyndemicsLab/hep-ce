////////////////////////////////////////////////////////////////////////////////
// File: simulation.cpp                                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-22                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-09-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/simulation.hpp>

#include <hepce/event/all_events.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/math.hpp>

#include "internals/simulation_internals.hpp"

namespace hepce {
namespace model {

//Factory
std::unique_ptr<Hepce> Hepce::Create(datamanagement::ModelData &model_data,
                                     const std::string &log_name) {
    return std::make_unique<HepceImpl>(model_data, log_name);
}

// Constructor
HepceImpl::HepceImpl(datamanagement::ModelData &model_data,
                     const std::string &log_name)
    : _log_name(log_name) {
    _duration = utils::GetIntFromConfig("simulation.duration", model_data);
    _sim_seed = utils::GetIntFromConfig("simulation.seed", model_data);
    if (_sim_seed < 0) {
        _sim_seed = utils::GetCurrentTimeInMilliseconds();
        std::stringstream msg;
        msg << "No seed or negative seed provided in `sim.conf`. Using "
               "generated seed value: "
            << _sim_seed << ".";
        hepce::utils::LogWarning(GetLogName(), msg.str());
    }
}

void HepceImpl::Run(
    std::vector<std::unique_ptr<model::Person>> &people,
    const std::vector<std::unique_ptr<event::Event>> &discrete_events) {
    auto sampler = hepce::model::Sampler::Create(GetSeed(), GetLogName());
#pragma omp parallel for
    for (auto &&person : people) {
        for (int i = 0; i < GetDuration(); ++i) {
            for (auto &&event : discrete_events) {
                event->Execute(*person, *sampler);
            }
        }
    }
}

std::vector<std::unique_ptr<event::Event>>
HepceImpl::CreateEvents(datamanagement::ModelData &model_data) const {
    std::vector<std::unique_ptr<event::Event>> events;
    auto event_strings = utils::SplitToVecT<std::string>(
        utils::GetStringFromConfig("simulation.events", model_data), ',');
    for (std::string e : event_strings) {
        events.push_back(CreateEvent(e, model_data));
    }
    return events;
}

std::vector<std::unique_ptr<model::Person>>
HepceImpl::CreatePopulation(datamanagement::ModelData &model_data) const {
    int population_size =
        utils::GetIntFromConfig("simulation.population_size", model_data);

    bool use_population_table =
        utils::GetBoolFromConfig("simulation.use_population_table", model_data);

    if (!use_population_table) {
        return ReadICPopulation(population_size, model_data);
    }
    return ReadPopPopulation(population_size, model_data);
}

std::vector<std::unique_ptr<model::Person>>
HepceImpl::ReadICPopulation(const int population_size,
                            datamanagement::ModelData &model_data) const {

    std::any storage = std::vector<data::PersonSelect>{};

    try {
        model_data.GetDBSource("inputs").Select(
            InitialCohortSQL(population_size), InitCohortVecCallback, storage);
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error drawing population size " << population_size
            << " from Initial Cohort. Using Default Person Values...";
        hepce::utils::LogWarning(GetLogName(), msg.str());
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        return {};
    }
    std::vector<data::PersonSelect> vec =
        std::any_cast<std::vector<data::PersonSelect>>(storage);
    std::vector<std::unique_ptr<model::Person>> population = {};
    for (const auto &ps : vec) {
        auto person = model::Person::Create(GetLogName());
        person->SetPersonDetails(ps);
        population.push_back(std::move(person));
    }
    return population;
}

std::vector<std::unique_ptr<model::Person>>
HepceImpl::ReadPopPopulation(const int population_size,
                             datamanagement::ModelData &model_data) const {
    std::stringstream query;
    std::vector<std::string> events = utils::SplitToVecT<std::string>(
        utils::GetStringFromConfig("simulation.events", model_data), ',');

    // this is a stopgap with plans to make Event-scoped CheckFor<X>Event
    // functions that are static / usable throughout the model.
    bool pregnancy = utils::FindInVector<std::string>(events, {"pregnancy"});
    bool hcc = utils::FindInVector<std::string>(events, {"HCCScreening"});
    bool overdose =
        utils::FindInVector<std::string>(events, {"BehaviorChanges"});
    bool hiv = utils::FindInVector<std::string>(
        events,
        {"HIVInfections", "HIVLinking", "HIVScreening", "HIVTreatment"});
    bool moud = utils::FindInVector<std::string>(events, {"MOUD"});

    // TODO: Add string santization (i.e. verify no extra special characters/numbers/phrases/etc.)
    query << "SELECT "
          << data::POPULATION_HEADERS(pregnancy, hcc, overdose, hiv, moud);
    query << "FROM population ";
    query << "ORDER BY id ";
    query << "LIMIT " << std::to_string(population_size) << ";";

    std::any storage = std::vector<data::PersonSelect>{};

    try {
        model_data.GetDBSource("inputs").Select(query.str(), PersonVecCallback,
                                                storage);
    } catch (std::exception &e) {
        std::stringstream msg;
        msg << "Error getting " << population_size
            << " people from Population File. Using Default Person Values...";
        hepce::utils::LogWarning(GetLogName(), msg.str());
#ifdef EXIT_ON_WARNING
        std::exit(EXIT_FAILURE);
#endif
        return {};
    }
    std::vector<data::PersonSelect> vec =
        std::any_cast<std::vector<data::PersonSelect>>(storage);
    std::vector<std::unique_ptr<model::Person>> population = {};
    for (const auto &ps : vec) {
        auto person = model::Person::Create(GetLogName());
        person->SetPersonDetails(ps);
        population.push_back(std::move(person));
    }
    return population;
}

std::unique_ptr<event::Event>
HepceImpl::CreateEvent(std::string event_name,
                       datamanagement::ModelData &model_data) const {
    if (event_name == "Aging") {
        return hepce::event::base::Aging::Create(model_data, GetLogName());
    }
    if (event_name == "BehaviorChanges") {
        return hepce::event::behavior::BehaviorChanges::Create(model_data,
                                                               GetLogName());
    }
    if (event_name == "Clearance") {
        return hepce::event::hcv::Clearance::Create(model_data, GetLogName());
    }
    if (event_name == "Death") {
        return hepce::event::base::Death::Create(model_data, GetLogName());
    }
    if (event_name == "FibrosisProgression") {
        return hepce::event::fibrosis::Progression::Create(model_data,
                                                           GetLogName());
    }
    if (event_name == "FibrosisStaging") {
        return hepce::event::fibrosis::Staging::Create(model_data,
                                                       GetLogName());
    }
    if (event_name == "HCVInfection") {
        return hepce::event::hcv::Infection::Create(model_data, GetLogName());
    }
    if (event_name == "HCVLinking") {
        return hepce::event::hcv::Linking::Create(model_data, GetLogName());
    }
    if (event_name == "HCVScreening") {
        return hepce::event::hcv::Screening::Create(model_data, GetLogName());
    }
    if (event_name == "HCVTreatment") {
        return hepce::event::hcv::Treatment::Create(model_data, GetLogName());
    }
    if (event_name == "HIVInfection") {
        return hepce::event::hiv::Infection::Create(model_data, GetLogName());
    }
    if (event_name == "HIVLinking") {
        return hepce::event::hiv::Linking::Create(model_data, GetLogName());
    }
    if (event_name == "HIVScreening") {
        return hepce::event::hiv::Screening::Create(model_data, GetLogName());
    }
    if (event_name == "Overdose") {
        return hepce::event::behavior::Overdose::Create(model_data,
                                                        GetLogName());
    }
    if (event_name == "VoluntaryRelinking") {
        return hepce::event::hcv::VoluntaryRelink::Create(model_data,
                                                          GetLogName());
    }
    if (event_name == "Pregnancy") {
        return hepce::event::behavior::Pregnancy::Create(model_data,
                                                         GetLogName());
    }
    if (event_name == "MOUD") {
        return hepce::event::behavior::Moud::Create(model_data, GetLogName());
    }
    std::string message =
        "Unknown Event `" + event_name + "` provided in `sim.conf`! Exiting...";
    hepce::utils::LogError(GetLogName(), message);
    exit(-1);
}
} // namespace model
} // namespace hepce

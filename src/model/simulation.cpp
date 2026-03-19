////////////////////////////////////////////////////////////////////////////////
// File: simulation.cpp                                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-22                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/simulation.hpp>

#include <hepce/data/inputs.hpp>
#include <hepce/event/event_factory.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
#include <hepce/utils/math.hpp>

#include "internals/simulation_internals.hpp"

namespace hepce {
namespace model {

//Factory
std::unique_ptr<Hepce> Hepce::Create(const data::Inputs &inputs,
                                     const std::string &log_name) {
    return std::make_unique<HepceImpl>(inputs, log_name);
}

// Constructor
HepceImpl::HepceImpl(const data::Inputs &inputs, const std::string &log_name)
    : _inputs(inputs), _log_name(log_name) {
    _duration = utils::GetIntFromConfig("simulation.duration", inputs);
    _sim_seed = utils::GetIntFromConfig("simulation.seed", inputs);
    if (_sim_seed < 0) {
        _sim_seed = utils::GetCurrentTimeInMilliseconds();
        std::stringstream msg;
        msg << "No seed or negative seed provided in `sim.conf`. Using "
               "generated seed value: "
            << _sim_seed << ".";
        hepce::utils::LogWarning(GetLogName(), msg.str());
    }
}

void HepceImpl::Run(const model::People &people,
                    const event::EventList &discrete_events) {
    auto sampler = hepce::model::Sampler::Create(GetSeed(), GetLogName());
#pragma omp parallel for
    for (const auto &person : people) {
        for (int i = 0; i < GetDuration(); ++i) {
            for (const auto &event : discrete_events) {
                event->Execute(*person, *sampler);
            }
        }
    }
}

event::EventList HepceImpl::CreateEvents() const {
    event::EventList events;
    auto event_strings = utils::SplitToVecT<std::string>(
        utils::GetStringFromConfig("simulation.events", _inputs), ',');
    for (std::string e : event_strings) {
        auto event = event::EventFactory::CreateEvent(e, _inputs, _log_name);
        events.push_back(std::move(event));
    }
    return events;
}

model::People HepceImpl::CreatePopulation() const {
    int population_size =
        utils::GetIntFromConfig("simulation.population_size", _inputs);

    bool use_population_table =
        utils::GetBoolFromConfig("simulation.use_population_table", _inputs);

    return (!use_population_table) ? ReadICPopulation(population_size)
                                   : ReadPopPopulation(population_size);
}

[[deprecated(
    "The Initial Cohort Table is deprecated. Please use the Population Table "
    "instead as it provides more flexibility and control of the data.")]]
model::People HepceImpl::ReadICPopulation(const int population_size) const {

    std::any storage = std::vector<data::PersonSelect>{};

    try {
        _inputs.SelectFromDatabase(InitialCohortSQL(population_size),
                                   InitCohortVecCallback, storage, {});

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
    model::People population = {};
    int start_time =
        utils::GetIntFromConfig("simulation.start_time", model_data);
    for (const auto &ps : vec) {
        auto person = model::Person::Create(GetLogName());
        person->SetPersonDetails(ps);
        person->SetStartTime(start_time);
        population.push_back(std::move(person));
    }
    return population;
}

model::People
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
    query << " FROM population ";
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
    model::People population = {};
    int start_time =
        utils::GetIntFromConfig("simulation.start_time", model_data);
    for (const auto &ps : vec) {
        auto person = model::Person::Create(GetLogName());
        person->SetPersonDetails(ps);
        person->SetStartTime(start_time);
        population.push_back(std::move(person));
    }
    return population;
}
} // namespace model
} // namespace hepce

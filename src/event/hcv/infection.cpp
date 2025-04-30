////////////////////////////////////////////////////////////////////////////////
// File: infection.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hcv/infection.hpp>

#include "internals/infection_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace hcv {

// Factory
std::unique_ptr<hepce::event::Event>
Infection::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<InfectionImpl>(model_data, log_name);
}

// Constructor
InfectionImpl::InfectionImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name = "console")
    : EventBase(model_data, log_name) {
    int rc = LoadIncidenceData(model_data);
    _gt3_prob =
        utils::GetDoubleFromConfig("infection.genotype_three_prob", model_data);
}

// Execute
int InfectionImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // Acute cases progress to chronic after 6 consecutive months of
    // infection
    if (person.GetHCVDetails().hcv == data::HCV::kAcute &&
        (person.GetCurrentTimestep() - person.GetHCVDetails().time_changed) ==
            6) {
        person.SetHCV(data::HCV::kChronic);
    }

    // If already infected, skip
    if (person.GetHCVDetails().hcv != data::HCV::kNone) {
        return;
    }

    // draw new infection probability
    std::vector<double> prob = GetInfectionProbability(person);
    // decide whether person is infected; if value == 0, infect
    if (sampler.GetDecision(prob) == 0) {
        person.InfectHCV();
        // decide whether hcv is genotype three
        if (sampler.GetDecision({_gt3_prob}) == 0) {
            person.SetGenotypeThree(true);
        }
    }
}

// Private Methods
std::vector<double>
InfectionImpl::GetInfectionProbability(const model::Person &person) {
    if (_infection_data.empty()) {
        // Warn Empty
        return {0.0};
    }

    int age_years = static_cast<int>(person.GetAge() / 12.0);
    int gender = static_cast<int>(person.GetSex());
    int drug_behavior = static_cast<int>(person.GetBehaviorDetails().behavior);
    utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
    double incidence = _infection_data[tup];

    return {incidence};
}

int InfectionImpl::LoadIncidenceData(datamanagement::ModelData &model_data) {
    std::string error;
    std::any storage = _infection_data;
    model_data.GetDBSource("inputs").Select(IncidenceSQL(), CallbackInfection,
                                            storage);
    _infection_data = std::any_cast<incidencemap_t>(storage);
    if (_infection_data.empty()) {
        // Warn Empty
    }
    return 0;
}

} // namespace hcv
} // namespace event
} // namespace hepce
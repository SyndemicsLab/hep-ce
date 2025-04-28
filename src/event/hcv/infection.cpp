////////////////////////////////////////////////////////////////////////////////
// File: infection.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
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
std::unique_ptr<hepce::event::Event>
Infection::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<InfectionImpl>(model_data, log_name);
}

InfectionImpl::InfectionImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name = "console") {
    SetDiscount(
        utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
    int rc = LoadIncidenceData(model_data);
    _gt3_prob =
        utils::GetDoubleFromConfig("infection.genotype_three_prob", model_data);
}

int InfectionImpl::Execute(model::Person &person, model::Sampler &sampler) {
    // Acute cases progress to chronic after 6 consecutive months of
    // infection
    if (person.GetHCV() == data::HCV::ACUTE &&
        person.GetTimeSinceHCVChanged() == 6) {
        person.SetHCV(data::HCV::CHRONIC);
    }

    // If already infected, skip
    if (person.GetHCV() != data::HCV::NONE) {
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
} // namespace hcv
} // namespace event
} // namespace hepce
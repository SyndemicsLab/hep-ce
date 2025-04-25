////////////////////////////////////////////////////////////////////////////////
// File: infection.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
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
Infection::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name) {
    return std::make_unique<InfectionImpl>(dm, log_name);
}

InfectionImpl::InfectionImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name = "console") {
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    int rc = LoadIncidenceData(dm);
    std::string data;
    dm->GetFromConfig("infection.genotype_three_prob", data);
    if (data.empty()) {
        // Warn Empty
    } else {
        _gt3_prob = utils::SToDPositive(data);
    }
}

int InfectionImpl::Execute(model::Person &person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           model::Sampler &sampler) {
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
    std::vector<double> prob = GetInfectionProbability(person, dm);
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
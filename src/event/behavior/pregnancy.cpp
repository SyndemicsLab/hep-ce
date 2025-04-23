////////////////////////////////////////////////////////////////////////////////
// File: pregnancy.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/behavior/pregnancy.hpp>

#include "internals/pregnancy_internals.hpp"
#include <hepce/utils/formatting.hpp>

namespace hepce {
namespace event {
namespace behavior {
PregnancyImpl::PregnancyImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name) {
    std::string storage;
    dm->GetFromConfig("pregnancy.multiple_delivery_probability", storage);
    if (storage.empty()) {
        // spdlog::get("main")->warn(
        //     "No Multiple Delivery Probability Found! Assuming 0.");
        storage = "0";
    }
    multiple_delivery_probability = utils::SToDPositive(storage);

    storage.clear();
    dm->GetFromConfig("pregnancy.infant_hcv_tested_probability", storage);
    if (storage.empty()) {
        // spdlog::get("main")->warn(
        //     "No Infant HCV Testing Probability Found! Assuming 0.");
        storage = "0";
    }
    infant_hcv_tested_probability = utils::SToDPositive(storage);

    storage.clear();
    dm->GetFromConfig("pregnancy.vertical_hcv_transition_probability", storage);
    if (storage.empty()) {
        // spdlog::get("main")->warn("No Infant HCV Vertical Transmission "
        //                           "Probability Found! Assuming 0.");
        storage = "0";
    }
    vertical_hcv_transition_probability = utils::SToDPositive(storage);

    LoadPregnancyData(dm);
}

int PregnancyImpl::Execute(model::Person &person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           model::Sampler &sampler) {
    if (person.GetSex() == data::Sex::MALE || person.GetAge() < 180 ||
        person.GetAge() > 540 ||
        (person.GetPregnancyState() == data::PregnancyState::POSTPARTUM &&
         person.GetTimeSincePregnancyChange() < 3)) {
        return;
    }

    if (person.GetPregnancyState() == data::PregnancyState::POSTPARTUM) {
        person.EndPostpartum();
    }

    if (person.GetPregnancyState() == data::PregnancyState::PREGNANT) {
        if (person.GetTimeSincePregnancyChange() >= 9) {
            AttemptHaveChild(person, sampler);
        } else {
            AttemptHealthyMonth(person, sampler);
        }
    } else {
        double prob =
            pregnancy_data[static_cast<int>(person.GetAge() / 12.0)].pregnant;
        if (sampler.GetDecision({1 - prob, prob})) {
            person.Impregnate();
        }
    }
    return 0;
}

std::unique_ptr<hepce::event::Event>
Pregnancy::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name) {
    return std::make_unique<PregnancyImpl>(dm, log_name);
}
} // namespace behavior
} // namespace event
} // namespace hepce
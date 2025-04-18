////////////////////////////////////////////////////////////////////////////////
// File: aging.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/base/aging.hpp>

#include "internals/aging_internals.hpp"
#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>
#include <hepce/utils/math.hpp>

namespace hepce {
namespace event {
namespace base {
int AgingImpl::Execute(model::Person &person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       model::Sampler &sampler) {
    person.Grow();
    AddBackgroundCostAndUtility(person, dm);
    person.AddDiscountedLifeSpan(
        utils::Discount(1, GetDiscount(), person.GetCurrentTimestep()));
}

std::unique_ptr<hepce::event::Event>
Aging::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
              const std::string &log_name) {
    return std::make_unique<AgingImpl>(dm, log_name);
}
} // namespace base
} // namespace event
} // namespace hepce
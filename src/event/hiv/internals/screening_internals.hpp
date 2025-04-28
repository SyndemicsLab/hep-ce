////////////////////////////////////////////////////////////////////////////////
// File: screening_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_

#include <hepce/event/hiv/screening.hpp>

#include <functional>

#include "internals/screening_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>
namespace hepce {
namespace event {
namespace hiv {
class ScreeningImpl : public virtual hiv::Screening, public ScreeningBase {
public:
    ScreeningImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");

    ~ScreeningImpl() = default;

private:
    data::InfectionType GetInfectionType() const override {
        return data:: ::kHiv;
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_
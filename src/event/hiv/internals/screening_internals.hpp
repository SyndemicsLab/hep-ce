////////////////////////////////////////////////////////////////////////////////
// File: screening_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_

// File Header
#include <hepce/event/hiv/screening.hpp>

// STL Includes
#include <functional>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "../../internals/screening_internals.hpp"
namespace hepce {
namespace event {
namespace hiv {
class ScreeningImpl : public virtual Screening, public ScreeningBase {
public:
    ScreeningImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");

    ~ScreeningImpl() = default;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHiv;
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_
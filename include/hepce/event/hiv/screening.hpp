////////////////////////////////////////////////////////////////////////////////
// File: screening.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_SCREENING_HPP_
#define HEPCE_EVENT_HIV_SCREENING_HPP_

#include <memory>
#include <string>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace hiv {
class Screening : public virtual Event {
public:
    virtual ~Screening() = default;

    static std::unique_ptr<Event>
    Create(datamanagement::ModelData &model_data,
           const std::string &log_name = "console");
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_SCREENINGS_HPP_
////////////////////////////////////////////////////////////////////////////////
// File: screening.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_SCREENING_HPP_
#define HEPCE_EVENT_HCV_SCREENING_HPP_

#include <memory>
#include <string>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace hcv {
class Screening : public virtual Event {
public:
    virtual ~Screening() = default;

    static std::unique_ptr<Event>
    Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
           const std::string &log_name = "console");
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_SCREENINGS_HPP_
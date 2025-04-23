////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/hcv/linking.hpp>

#include "hcv/internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {
std::unique_ptr<hepce::event::Event>
hcv::Linking::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                     const std::string &log_name) {
    return std::make_unique<hcv::LinkingImpl>(dm, log_name);
}
} // namespace hcv
} // namespace event
} // namespace hepce
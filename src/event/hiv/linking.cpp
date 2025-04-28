////////////////////////////////////////////////////////////////////////////////
// File: linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: We Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/hiv/linking.hpp>

#include "hiv/internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
std::unique_ptr<hepce::event::Event>
hiv::Linking::Create(datamanagement::ModelData &model_data,
                     const std::string &log_name) {
    return std::make_unique<hiv::LinkingImpl>(model_data, log_name);
}
} // namespace hiv
} // namespace event
} // namespace hepce
////////////////////////////////////////////////////////////////////////////////
// File: Treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-16                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Treatment.hpp"

namespace event {
TreatmentIMPL::TreatmentIMPL(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    this->discount = Utils::GetDoubleFromConfig("cost.discounting_rate", dm);
}
} // namespace event

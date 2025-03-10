////////////////////////////////////////////////////////////////////////////////
// File: PersonFactory.cpp                                                    //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "PersonFactory.hpp"

namespace person {
std::shared_ptr<person::PersonBase> PersonFactory::create() {
    return std::make_shared<person::Person>();
}
} // namespace person

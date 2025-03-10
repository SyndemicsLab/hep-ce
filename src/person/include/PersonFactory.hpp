////////////////////////////////////////////////////////////////////////////////
// File: PersonFactory.hpp                                                    //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef PERSONFACTORY_HPP_
#define PERSONFACTORY_HPP_

#include "Person.hpp"
#include <memory>

namespace person {
class PersonFactory {
public:
    std::shared_ptr<person::PersonBase> create();
};
} // namespace person

#endif

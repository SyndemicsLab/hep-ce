//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Simulation Class.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef DATAWRITER_HPP_
#define DATAWRITER_HPP_

#include "Cost.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <DataManagement.hpp>

/// @brief Namespace containing functions for writing simulation output
namespace DataWriter {
    /// @brief Write out the population after the simulation is complete
    /// @param population Vector of shared pointers to all Person objects in the
    /// simulation
    /// @param dirpath Path to the output folder
    void
    writePopulation(std::vector<std::shared_ptr<Person::Person>> &population,
                    std::string dirpath);

    /// @brief Function that takes attributes from a Person object and places
    /// them into a DataTable row
    /// @param person Pointer to the current Person object
    /// @return Pointer to a DataTable row
    Data::IDataTablePtr
    personToDataTable(std::shared_ptr<Person::Person> person);

    void
    writeGeneralStats(std::vector<std::shared_ptr<Person::Person>> &population,
                      std::string dirpath, Data::Config &config);
} // namespace DataWriter
#endif

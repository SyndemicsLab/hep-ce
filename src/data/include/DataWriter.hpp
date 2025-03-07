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

#include <memory>
#include <vector>

namespace person {
class PersonBase;
}
namespace datamanagement {
class DataManagerBase;
}

/// @brief Namespace containing functions for writing simulation output
namespace writer {
class DataWriter {
private:
    class DataWriterIMPL;
    std::unique_ptr<DataWriterIMPL> impl;

public:
    DataWriter();
    ~DataWriter();
    int UpdatePopulation(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::shared_ptr<datamanagement::DataManagerBase> dm);
    int WritePopulationToFile(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::string &filepath);
};
} // namespace writer
#endif

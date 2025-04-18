////////////////////////////////////////////////////////////////////////////////
// File: DataWriter.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-06-12                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

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
    std::string PopulationToString(
        std::vector<std::shared_ptr<person::PersonBase>> population);
    int WritePopulationToFile(
        std::vector<std::shared_ptr<person::PersonBase>> new_population,
        std::string &filepath);
};
} // namespace writer
#endif

////////////////////////////////////////////////////////////////////////////////
// File: writer.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/writer_internals.hpp"

#include <filesystem>
#include <fstream>

#include <hepce/model/person.hpp>

namespace hepce {
namespace data {

std::unique_ptr<Writer> Writer::Create(const std::string &directory,
                                       const std::string &log_name) {
    return std::make_unique<WriterImpl>(directory, log_name);
}

std::string
WriterImpl::WritePopulation(const std::vector<model::Person> &population,
                            const std::string &filename,
                            const OutputType output_type) {
    std::filesystem::path path = filename;
    std::ofstream csvStream;
    csvStream.open(path, std::ofstream::out);
    if (!csvStream) {
        return "";
    }
    csvStream << "id," << POPULATION_HEADERS() << std::endl;
    for (const auto &person : population) {
        csvStream << person.MakePopulationRow() << std::endl;
    }
    csvStream.close();
    return 0;
}
} // namespace data
} // namespace hepce
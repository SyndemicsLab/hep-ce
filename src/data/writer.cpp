////////////////////////////////////////////////////////////////////////////////
// File: writer.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/writer_internals.hpp"

#include <filesystem>
#include <fstream>
#include <numeric>

#include <hepce/model/person.hpp>

namespace hepce {
namespace data {

std::unique_ptr<Writer> Writer::Create(const std::string &directory,
                                       const std::string &log_name) {
    return std::make_unique<WriterImpl>(directory, log_name);
}

WriterImpl::WriterImpl(const std::string &directory,
                       const std::string &log_name) {
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directory(directory);
    }
}

std::string WriterImpl::WritePopulation(
    const std::vector<std::unique_ptr<model::Person>> &population,
    const std::string &filename, const OutputType output_type,
    std::vector<int> ids) {
    if (ids.empty()) {
        ids.resize(population.size());
        std::iota(ids.begin(), ids.end(), 1);
    }
    std::filesystem::path path = filename;
    std::ofstream csvStream;
    csvStream.open(path, std::ofstream::out);
    if (!csvStream) {
        return "";
    }
    csvStream << "id," << POPULATION_HEADERS(true, true, true, true, true)
              << ",cost,discount_cost" << std::endl;
    for (int i = 0; i < population.size(); ++i) {
        csvStream << ids[i] << "," << population[i]->MakePopulationRow()
                  << std::endl;
    }
    csvStream.close();
    return "success";
}
} // namespace data
} // namespace hepce
////////////////////////////////////////////////////////////////////////////////
// File: writer.cpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-29                                                  //
// Modified By: Andrew Clark                                                  //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/writer_internals.hpp"

#include <filesystem>
#include <fstream>
#include <numeric>

#include <hepce/model/costing.hpp>
#include <hepce/model/person.hpp>
#include <hepce/utils/logging.hpp>

namespace hepce {
namespace data {

std::unique_ptr<Writer> Writer::Create(const std::string &directory,
                                       const std::string &log_name) {
    return std::make_unique<WriterImpl>(directory, log_name);
}

WriterImpl::WriterImpl(const std::string &directory,
                       const std::string &log_name)
    : _log_name(log_name) {
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
        hepce::utils::LogError(GetLogName(),
                               "Unable to open CSV Stream to write!");
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

std::string WriterImpl::WriteCostsByCategory(
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
        hepce::utils::LogError(GetLogName(),
                               "Unable to open CSV Stream to write!");
        return "";
    }
    csvStream << "id,"
              << "misc,discount_misc,behavior,discount_behavior,screening,"
              << "discount_screening,linking,discount_linking,staging,"
              << "discount_staging,liver,discount_liver,treatment,"
              << "discount_treatment,background,discount_background,"
              << "hiv,discount_hiv" << std::endl;
    for (int i = 0; i < population.size(); ++i) {
        csvStream << ids[i] << ",";
        const auto &person_costs = population[i]->GetCosts();
        for (int j = 0;
             j < static_cast<int>(hepce::model::CostCategory::kCount); ++j) {
            const auto &category_cost =
                person_costs.at(static_cast<hepce::model::CostCategory>(j));
            csvStream << category_cost.first << "," << category_cost.second;
            if (static_cast<hepce::model::CostCategory>(j + 1) ==
                hepce::model::CostCategory::kCount) {
                break;
            }
            csvStream << ",";
        }
        csvStream << std::endl;
    }
    csvStream.close();
    return "success";
}
} // namespace data
} // namespace hepce

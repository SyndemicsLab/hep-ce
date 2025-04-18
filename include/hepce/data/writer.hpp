////////////////////////////////////////////////////////////////////////////////
// File: writer.hpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_WRITER_HPP_
#define HEPCE_DATA_WRITER_HPP_

#include <memory>
#include <string>
#include <vector>

#include <hepce/model/person.hpp>

namespace hepce {
namespace data {
enum class OutputType : int { kString = 0, kFile = 1, kCount = 2 };
class Writer {
public:
    virtual ~Writer() = default;
    virtual std::string
    WritePopulation(const std::vector<model::Person> &population,
                    const std::string &filename,
                    const OutputType output_type) = 0;

    static std::unique_ptr<Writer>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};
} // namespace data
} // namespace hepce

#endif
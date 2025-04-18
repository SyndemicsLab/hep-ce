////////////////////////////////////////////////////////////////////////////////
// File: writer_internals.hpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_WRITERINTERNALS_HPP_
#define HEPCE_DATA_WRITERINTERNALS_HPP_

#include <hepce/data/writer.hpp>

namespace hepce {
namespace data {
class WriterImpl : public virtual Writer {
public:
    WriterImpl(const std::string &directory = "",
               const std::string &log_name = "console") {}
    ~WriterImpl() = default;
    std::string WritePopulation(const std::vector<model::Person> &population,
                                const std::string &filename,
                                const OutputType output_type) override;
};
} // namespace data
} // namespace hepce

#endif // HEPCE_DATA_WRITERINTERNALS_HPP_
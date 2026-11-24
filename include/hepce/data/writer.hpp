////////////////////////////////////////////////////////////////////////////////
// File: writer.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_WRITER_HPP_
#define HEPCE_DATA_WRITER_HPP_

#include <memory>
#include <string>
#include <vector>

#include <hepce/model/person.hpp>

namespace hepce {
namespace data {
/// Selects whether output is returned as a string or written to a file.
enum class OutputType : int { kString = 0, kFile = 1, kCount = 2 };
/// Defines output operations for a simulated population.
class Writer {
public:
    /// Destroys the writer.
    virtual ~Writer() = default;
    /// Writes population-level person data.
    /// @param population People whose data should be written.
    /// @param filename Output filename when writing to a file.
    /// @param output_type Selects string or file output.
    /// @param ids Optional person IDs to include.
    /// @return The generated output or an operation status string.
    virtual std::string WritePopulation(const model::People &population,
                                        const std::string &filename,
                                        const OutputType output_type,
                                        std::vector<int> ids = {}) = 0;

    /// Writes population costs grouped by cost category.
    /// @param population People whose costs should be written.
    /// @param filename Output filename when writing to a file.
    /// @param output_type Selects string or file output.
    /// @param ids Optional person IDs to include.
    /// @return The generated output or an operation status string.
    virtual std::string WriteCostsByCategory(const model::People &population,
                                             const std::string &filename,
                                             const OutputType output_type,
                                             std::vector<int> ids = {}) = 0;

    /// Creates the concrete population writer.
    /// @param directory Directory used for file output.
    /// @param log_name Logger name used by the writer.
    /// @return A new writer instance.
    static std::unique_ptr<Writer>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};
} // namespace data
} // namespace hepce

#endif

////////////////////////////////////////////////////////////////////////////////
// File: writer_internals.hpp                                                 //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_WRITERINTERNALS_HPP_
#define HEPCE_DATA_WRITERINTERNALS_HPP_

#include <hepce/data/writer.hpp>

namespace hepce {
namespace data {
class WriterImpl : public virtual Writer {
public:
    WriterImpl(const std::string &directory = "",
               const std::string &log_name = "console");
    ~WriterImpl() = default;
    std::string WritePopulation(const model::People &population,
                                const std::string &filename,
                                const OutputType output_type,
                                std::vector<int> ids = {}) override;
    std::string WriteCostsByCategory(const model::People &population,
                                     const std::string &filename,
                                     const OutputType output_type,
                                     std::vector<int> ids = {}) override;

protected:
    const std::string GetLogName() const { return _log_name; }

private:
    const std::string _log_name;
};
} // namespace data
} // namespace hepce

#endif // HEPCE_DATA_WRITERINTERNALS_HPP_

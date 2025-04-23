////////////////////////////////////////////////////////////////////////////////
// File: sampler.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SAMPLER_HPP_
#define HEPCE_MODEL_SAMPLER_HPP_

#include <memory>
#include <random>
#include <string>

namespace hepce {
namespace model {
class Sampler {
public:
    virtual ~Sampler() = default;
    virtual const int GetDecision(const std::vector<double> &probs) = 0;

    static std::unique_ptr<Sampler>
    Create(const int &seed, const std::string &log_name = "console");
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SAMPLER_HPP_
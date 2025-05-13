////////////////////////////////////////////////////////////////////////////////
// File: sampler_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SAMPLERINTERNALS_HPP_
#define HEPCE_MODEL_SAMPLERINTERNALS_HPP_

#include <hepce/model/sampler.hpp>

#include <random>
#include <string>
#include <vector>

namespace hepce {
namespace model {
class SamplerImpl : public virtual Sampler {
public:
    SamplerImpl(const int &seed, const std::string &log_name);
    ~SamplerImpl() = default;
    const int GetDecision(const std::vector<double> &probs) override;

private:
    const std::string _log_name;
    std::mt19937_64 _generator;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SAMPLERINTERNALS_HPP_
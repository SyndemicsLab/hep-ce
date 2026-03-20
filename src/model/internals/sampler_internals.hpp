////////////////////////////////////////////////////////////////////////////////
// File: sampler_internals.hpp                                                //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
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

    // Internal Constructor
    SamplerImpl(const std::mt19937_64 &g, const std::string &log_name)
        : _generator(g), _log_name(log_name) {}

    // Cloning
    std::unique_ptr<Sampler> clone() const override {
        return std::make_unique<SamplerImpl>(_generator, _log_name);
    }
    const int GetDecision(const std::vector<double> &probs) const override;

private:
    const std::string _log_name;
    mutable std::mt19937_64 _generator;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SAMPLERINTERNALS_HPP_
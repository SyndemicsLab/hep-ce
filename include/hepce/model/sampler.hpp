////////////////////////////////////////////////////////////////////////////////
// File: sampler.hpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_SAMPLER_HPP_
#define HEPCE_MODEL_SAMPLER_HPP_

#include <memory>
#include <string>
#include <vector>

namespace hepce {
namespace model {
/// Base interface for random decisions used by simulation events.
class Sampler {
public:
    /// Destroys the sampler.
    virtual ~Sampler() = default;

    Sampler(const Sampler &) = delete;
    Sampler &operator=(const Sampler &) = delete;
    /// Creates an independent copy of the sampler.
    virtual std::unique_ptr<Sampler> clone() const = 0;

    /// Creates a sampler with a deterministic seed.
    /// @param seed Random-number generator seed.
    /// @param log_name Logger name used by the sampler.
    /// @return A new sampler instance.
    static std::unique_ptr<Sampler>
    Create(const int &seed, const std::string &log_name = "console");

    /// Selects an index according to the supplied probability weights.
    /// @param probs Probability weights for each decision.
    /// @return The selected decision index.
    virtual const int GetDecision(const std::vector<double> &probs) const = 0;

protected:
    Sampler() = default;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_SAMPLER_HPP_
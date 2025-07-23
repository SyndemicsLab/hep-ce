////////////////////////////////////////////////////////////////////////////////
// File: sampler.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-05-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/sampler.hpp>

#include <algorithm>
#include <hepce/utils/logging.hpp>
#include <sstream>

#include "internals/sampler_internals.hpp"

namespace hepce {
namespace model {
std::unique_ptr<Sampler> Sampler::Create(const int &seed,
                                         const std::string &log_name) {
    return std::make_unique<SamplerImpl>(seed, log_name);
}

SamplerImpl::SamplerImpl(const int &seed, const std::string &log_name)
    : _log_name(log_name) {
    _generator.seed(seed);
}

const int SamplerImpl::GetDecision(const std::vector<double> &probabilities) {
    if (std::accumulate(probabilities.begin(), probabilities.end(), 0.0) >
        1.00001) {
        hepce::utils::LogError(
            _log_name,
            "Attempted to draw sample with probability sum greater than 1...");
        std::stringstream ss;
        std::for_each(probabilities.begin(), probabilities.end(),
                      [&](double val) { ss << val << " "; });
        hepce::utils::LogDebug(_log_name, "Probabilities are: " + ss.str());
        return -1;
    }
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double value = uniform(_generator);
    double reference = 0.0;
    for (int i = 0; i < probabilities.size(); ++i) {
        reference += probabilities[i];
        if (value < reference) {
            return i;
        }
    }
    return static_cast<int>(probabilities.size());
}
} // namespace model
} // namespace hepce

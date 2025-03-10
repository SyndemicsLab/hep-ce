////////////////////////////////////////////////////////////////////////////////
// File: Decider.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef STATS_DECIDER_HPP_
#define STATS_DECIDER_HPP_

#include <memory>
#include <random>
#include <vector>

namespace stats {
class DeciderBase {
public:
    virtual int LoadGenerator(std::mt19937_64 const generator) = 0;
    virtual int GetDecision(std::vector<double> probs) = 0;
};
class Decider : public DeciderBase {
private:
    class DeciderIMPL;
    std::unique_ptr<DeciderIMPL> impl;

public:
    Decider();
    ~Decider();
    int LoadGenerator(std::mt19937_64 const generator);
    int GetDecision(std::vector<double> probs);

    Decider(Decider const &) = delete;
    Decider &operator=(Decider const &) = delete;

    Decider(Decider &&) noexcept;
    Decider &operator=(Decider &&) noexcept;
};
} // namespace stats

#endif

#ifndef STATS_DECIDER_HPP_
#define STATS_DECIDER_HPP_

#include <memory>
#include <random>
#include <vector>

namespace stats {
    class Decider {
    private:
        class DeciderIMPL;
        std::unique_ptr<DeciderIMPL> impl;

    public:
        Decider();
        ~Decider();
        int LoadGenerator(std::mt19937_64 const generator);
        int GetDecision(std::vector<double> probs);
    };
} // namespace stats

#endif
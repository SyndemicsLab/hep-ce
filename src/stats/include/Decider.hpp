#ifndef STATS_DECIDER_HPP_
#define STATS_DECIDER_HPP_

#include <memory>
#include <random>
#include <vector>

namespace stats {
    class Decider {
    private:
        class DeciderIMPL;
        std::shared_ptr<DeciderIMPL> impl;
        std::mutex m;
        std::mt19937_64 g;

    public:
        Decider(std::mt19937_64 generator);
        ~Decider() = default;
        int GetDecision(std::vector<double> probs);
    };
} // namespace stats

#endif
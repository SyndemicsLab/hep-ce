#include "Decider.hpp"
#include <mutex>
#include <random>

namespace stats {
    class Decider::DeciderIMPL {
    public:
        int getDecision(std::vector<double> probs, std::mt19937_64 &g,
                        std::mutex &m) {
            if (std::accumulate(probs.begin(), probs.end(), 0.0) > 1.00001) {
                return -1;
            }
            std::uniform_real_distribution<double> uniform(0.0, 1.0);
            m.lock();
            double value = uniform(g);
            m.unlock();
            double reference = 0.0;
            for (int i = 0; i < probs.size(); ++i) {
                reference += probs[i];
                if (value < reference) {
                    return i;
                }
            }
            return (int)probs.size();
        }
    };

    Decider::Decider(std::mt19937_64 generator) : g(generator) {
        impl = std::make_shared<DeciderIMPL>();
    }

    int Decider::GetDecision(std::vector<double> probs) {
        return impl->getDecision(probs, this->g, this->m);
    }
} // namespace stats
#include "Decider.hpp"
#include <mutex>
#include <random>

namespace stats {
    class Decider::DeciderIMPL {
    private:
        std::mutex m;
        std::mt19937_64 g;

    public:
        int getDecision(std::vector<double> probs) {
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
        int LoadGenerator(std::mt19937_64 const generator) {
            g = generator;
            return 0;
        }
    };

    Decider::Decider() : impl(std::make_unique<DeciderIMPL>()) {}

    Decider::~Decider() = default;

    Decider::Decider(Decider &&) noexcept = default;
    Decider &Decider::operator=(Decider &&) noexcept = default;

    int Decider::LoadGenerator(std::mt19937_64 const generator) {
        return impl->LoadGenerator(generator);
    }

    int Decider::GetDecision(std::vector<double> probs) {
        return impl->getDecision(probs);
    }
} // namespace stats
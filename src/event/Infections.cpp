#include "Infections.hpp"

namespace Event {
    void Infections::doEvent(Person::Person &person) {

        // if person is already infected we can't infect them more
        if (person.getHEPCState() != Person::HEPCState::NONE) {
            return;
        }

        // draw new infection probability
        double prob = 0.5;
        std::bernoulli_distribution backgroundProbability(prob);
        this->generatorMutex.lock();
        int value = backgroundProbability(this->generator);
        this->generatorMutex.unlock();

        if (!value) {
            return;
        }

        person.infect();
    }
} // namespace Event
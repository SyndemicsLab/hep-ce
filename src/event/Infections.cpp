#include "Infections.hpp"

namespace Event {
    void Infections::doEvent(std::shared_ptr<Person::Person> person) {
        // if person is already infected we can't infect them more
        if (person->getHEPCState() != Person::HEPCState::NONE) {
            return;
        }

        // draw new infection probability
        std::vector<double> prob = this->getInfectProb(person);
        // decide whether person is infected; if value == 1, infect
        int value = this->getDecision(prob);
        if (!value) {
            return;
        }
        person->infect();
    }

    std::vector<double>
    Infections::getInfectProb(std::shared_ptr<Person::Person> person) {
        // need to add actual functionality
        // query the sqlite database based on infection probability strata
        return {0.5};
    }
} // namespace Event

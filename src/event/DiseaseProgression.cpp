#include "DiseaseProgression.hpp"

namespace Event {
    void DiseaseProgression::doEvent(Person::Person &person) {
        // 1. Get current disease status
        Person::LiverState ls = person.getLiverState();
        // 2. Get the transition probabilities from that state
        // std::vector<double> probs = someFunction(ls);
        // currently using placeholders to test compiling
        std::vector<double> probs = { 0.2, 0.2, 0.2, 0.2, 0.2 };
        // 3. Randomly draw the state to transition to
        Person::LiverState toLS =
            (Person::LiverState)this->getDecision(probs);
        // 4. Transition to the new state
        person.updateLiver(toLS);
    }
} // namespace Event

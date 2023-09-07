#include "BehaviorChanges.hpp"

namespace Event {
    void BehaviorChanges::doEvent(Person::Person &person) {
        // 1. Determine person's current behavior classification
        Person::BehaviorClassification bc = person.getBehaviorClassification();
        // 2. Generate the transition probabilities based on the starting state
        // std::vector<double> probs = getTransitions(bc);
        // currently using placeholders to test compiling
        std::vector<double> probs = {0.25, 0.25, 0.25, 0.25};
        // 3. Draw a behavior state to be transitioned to
        Person::BehaviorClassification toBC =
            (Person::BehaviorClassification)this->getDecision(probs);
        // 4. If the drawn state differs from the current state, change the
        // bools in BehaviorState to match
        person.updateBehavior(toBC);
    }
} // namespace Event

#include "BehaviorChanges.hpp"

namespace Event {
    void BehaviorChanges::doEvent(Person::Person &person) {
        // 1. Determine person's current behavior classification
        // BehaviorClassification &bc = person.getBehaviorClassification();
        // 2. Generate the transition probabilities based on the starting state
        // std::vector<double> probs = getTransitions(bc);
        // 3. Draw a behavior state to be transitioned to
        // BehaviorClassification toBehaviorClassification =
        //     (BehaviorClassification)this->getDecision(probs);
        // 4. If the drawn state differs from the current state, change the
        // bools in BehaviorState to match
        // person.updateBehavior(toBehaviorClassification);
    }
} // namespace Event

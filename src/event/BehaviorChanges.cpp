#include "BehaviorChanges.hpp"

namespace Event {
    void BehaviorChanges::doEvent(Person::Person &person) {
        // 1. Determine person's current behavior classification
        // BehaviorClassification &bc = person.getBehaviorClassification();
        // 2. Draw a behavior state to be transitioned to
        // BehaviorClassification toBehaviorClassification = someFunction(bc,
        // person);
        // 3. If the drawn state differs from the current state, change the
        // bools in BehaviorState to match
        // person.updateBehavior(toBehaviorClassification);
    }
} // namespace Event

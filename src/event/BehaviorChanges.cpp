#include "BehaviorChanges.hpp"

namespace Event {
    void BehaviorChanges::doEvent(std::shared_ptr<Person::Person> person) {
        // 1. Determine person's current behavior classification
        Person::BehaviorClassification bc = person->getBehaviorClassification();
        // 2. Generate the transition probabilities based on the starting state
        // std::vector<double> probs = getTransitions(bc);
        // currently using placeholders to test compiling
        std::vector<double> probs = {0.25, 0.25, 0.25, 0.25};
        // 3. Draw a behavior state to be transitioned to
        Person::BehaviorClassification toBC =
            (Person::BehaviorClassification)this->getDecision(probs);
        // 4. If the drawn state differs from the current state, change the
        // bools in BehaviorState to match
        person->updateBehavior(toBC);
    }

    std::vector<double>
    BehaviorChanges::getTransitions(std::shared_ptr<Person::Person> person) {
        // 1. Determine what strata impact behavior transitions
        // 2. Implement a way to generate the query string
        std::string query = "";
        Data::SQLTable result = this->db.readTable(query);
        return {};
    }
} // namespace Event

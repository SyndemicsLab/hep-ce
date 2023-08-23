#include "Treatment.hpp"

namespace Event {
    void Treatment::doEvent(Person::Person &person) {
        Person::FibrosisState personFibrosisState = person.getFibrosisState();
        if (person.getHEPCState() == Person::HEPCState::NONE &&
            personFibrosisState == Person::FibrosisState::NONE) {
            return;
        }

        if (personFibrosisState == Person::FibrosisState::DECOMP ||
            personFibrosisState == Person::FibrosisState::F4) {
            // assign cirrhotic state?
        } else {
            // assign non-cirrhotic state?
        }
        // Need to do stuff for building a treatment
    }
} // namespace Event

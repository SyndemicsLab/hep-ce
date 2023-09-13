#include "Treatment.hpp"

namespace Event {
    void Treatment::doEvent(std::shared_ptr<Person::Person> person) {
        Person::LiverState personLiverState = person->getLiverState();
        if (person->getHEPCState() == Person::HEPCState::NONE &&
            personLiverState == Person::LiverState::NONE) {
            return;
        }

        if (personLiverState == Person::LiverState::DECOMP ||
            personLiverState == Person::LiverState::F4) {
            // assign cirrhotic state?
        } else {
            // assign non-cirrhotic state?
        }
        // Need to do stuff for building a treatment
    }
} // namespace Event

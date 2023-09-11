#include "Aging.hpp"

namespace Event {
    void Aging::doEvent(std::shared_ptr<Person::Person> person) {
        if (person->getIsAlive()) {
            double monthAge = 1.0 / 12.0;
            person->age += monthAge;
        }
    }
} // namespace Event

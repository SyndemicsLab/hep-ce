#include "Aging.hpp"

namespace Event {
    void Aging::doEvent(Person::Person &person) {
        if (person.getIsAlive()) {
            person.age += (1 / 12);
        }
    }
} // namespace Event

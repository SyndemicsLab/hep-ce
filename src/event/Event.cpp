#include "Event.hpp"
#include "Person.hpp"

namespace event {
    int Event::Execute(person::PersonBase &person) {
        if (person.IsAlive()) {
            this->doEvent(person);
        }
        return 0;
    }
} // namespace event
#include "Death.hpp"

namespace Event {
    void Death::doEvent(std::shared_ptr<Person::Person> person) {
        if (person->age >= 1200) {
            this->die(person);
        }

        // "Calculate background mortality rate based on age, gender, and IDU"
        double backgroundMortality = 1.0;
    }

    void Death::die(std::shared_ptr<Person::Person> person) {
        // do the death thing here
    }
} // namespace Event

#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {

class Treatment : public Event {
private:
    void doEvent(Person::Person &person) override;
public:
    Treatment(){};
    virtual ~Treatment() = default;
};

}

#endif
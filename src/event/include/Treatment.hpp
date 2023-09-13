#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {

    class Treatment : public Event {
    private:
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        Treatment(){};
        virtual ~Treatment() = default;
    };

} // namespace Event

#endif
#ifndef EVENT_GROWTH_HPP_
#define EVENT_GROWTH_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {
    class Growth : public Event {
    private:
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        Growth(){};
        virtual ~Growth() = default;
    };
} // namespace Event

#endif

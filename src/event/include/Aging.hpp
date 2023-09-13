#ifndef EVENT_AGING_HPP_
#define EVENT_AGING_HPP_

#include "Event.hpp"

namespace Event {
    class Aging : public Event {
    private:
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        Aging(){};
        virtual ~Aging() = default;
    };
} // namespace Event

#endif

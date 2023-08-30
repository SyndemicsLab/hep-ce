#ifndef EVENT_BEHAVIORCHANGES_HPP_
#define EVENT_BEHAVIORCHANGES_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {
    class BehaviorChanges : public Event {
    private:
        void doEvent(Person::Person &person) override;

    public:
        BehaviorChanges(){};
        virtual ~BehaviorChanges() = default;
    };
} // namespace Event
#endif // EVENT_BEHAVIORCHANGES_HPP_

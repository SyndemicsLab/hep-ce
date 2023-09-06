#ifndef EVENT_BEHAVIORCHANGES_HPP_
#define EVENT_BEHAVIORCHANGES_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {
    class BehaviorChanges : public ProbEvent {
    private:
        void doEvent(Person::Person &person) override;

    public:
        using ProbEvent::ProbEvent;
        virtual ~BehaviorChanges() = default;
    };
} // namespace Event
#endif // EVENT_BEHAVIORCHANGES_HPP_

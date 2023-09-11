#ifndef EVENT_BEHAVIORCHANGES_HPP_
#define EVENT_BEHAVIORCHANGES_HPP_

#include "Event.hpp"
#include "Person.hpp"
#include <string>
#include <vector>

namespace Event {
    class BehaviorChanges : public ProbEvent {
    private:
        void doEvent(std::shared_ptr<Person::Person> person) override;
        std::vector<double> getTransitions(std::shared_ptr<Person::Person> person);

    public:
        using ProbEvent::ProbEvent;
        virtual ~BehaviorChanges() = default;
    };
} // namespace Event
#endif // EVENT_BEHAVIORCHANGES_HPP_

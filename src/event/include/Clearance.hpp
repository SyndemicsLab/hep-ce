#ifndef EVENT_CLEARANCE_HPP_
#define EVENT_CLEARANCE_HPP_
#include "Event.hpp"
#include "Person.hpp"

namespace Event {
    class Clearance : public ProbEvent {
    private:
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        using ProbEvent::ProbEvent;
        virtual ~Clearance() = default;
    };
} // namespace Event
#endif // EVENT_CLEARANCE_HPP_

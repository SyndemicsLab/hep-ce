#ifndef EVENT_DISEASEPROGRESSION_HPP_
#define EVENT_DISEASEPROGRESSION_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {
    class DiseaseProgression : public ProbEvent {
    private:
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        using ProbEvent::ProbEvent;
        virtual ~DiseaseProgression() = default;
    };
} // namespace Event
#endif

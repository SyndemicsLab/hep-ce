#ifndef EVENT_DISEASEPROGRESSION_HPP_
#define EVENT_DISEASEPROGRESSION_HPP_

#include "Event.hpp"
#include "Person.hpp"

namespace Event {

    class DiseaseProgression : public Event {
    private:
        void doEvent(Person::Person &person) override;

    public:
        DiseaseProgression(){};
        virtual ~DiseaseProgression() = default;
    };

} // namespace Event

#endif
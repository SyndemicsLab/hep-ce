#include "Event.hpp"
#include "Person.hpp"

namespace event {
    int Event::Execute(person::PersonBase &person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::unique_ptr<stats::Decider> &decider) {
        if (person.IsAlive()) {
            this->doEvent(person, dm, decider);
        }
        return 0;
    }

    Event::Event() {}

    Event::~Event() = default;
    Event::Event(Event &&) noexcept = default;
    Event &Event::operator=(Event &&) noexcept = default;
} // namespace event
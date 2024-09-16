#ifndef EVENTFACTORY_HPP_
#define EVENTFACTORY_HPP_

#include "Decider.hpp"
#include "Event.hpp"
#include <memory>
#include <string>

namespace datamanagement {
    class DataManager;
}
namespace event {

    class EventFactory {
    public:
        std::shared_ptr<Event> &
        create(std::shared_ptr<stats::Decider> decider,
               std::shared_ptr<datamanagement::DataManager> dm,
               std::string const eventName);
    };
} // namespace event

#endif

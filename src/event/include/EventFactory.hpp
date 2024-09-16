#ifndef EVENTFACTORY_HPP_
#define EVENTFACTORY_HPP_

#include "Event.hpp"
#include <memory>
#include <string>

namespace datamanagement {
    class DataManager;
}
namespace event {

    class EventFactory {
    private:
        /// @brief
        /// @tparam T
        /// @param generator
        /// @param table
        /// @return
        template <typename T>
        std::shared_ptr<Event>
        makeEvent(std::shared_ptr<stats::Decider> decider,
                  std::shared_ptr<datamanagement::DataManager> dm,
                  std::string name = "Event") {
            return std::make_shared<T>(decider, dm, name);
        }

    public:
        std::shared_ptr<Event>
        create(std::shared_ptr<stats::Decider> decider,
               std::shared_ptr<datamanagement::DataManager> dm,
               std::string const eventName);
    };
} // namespace event

#endif

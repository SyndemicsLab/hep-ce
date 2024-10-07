#ifndef EVENTFACTORY_HPP_
#define EVENTFACTORY_HPP_

#include "Event.hpp"
#include <memory>
#include <string>

namespace datamanagement {
    class DataManagerBase;
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
        makeEvent(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            return std::make_shared<T>(dm);
        }

    public:
        std::shared_ptr<Event>
        create(std::string eventName,
               std::shared_ptr<datamanagement::DataManagerBase> dm);
    };
} // namespace event

#endif

#ifndef EVENT_SCREENAGELINKING_HPP_
#define EVENT_SCREENAGELINKING_HPP_

#include "Event.hpp"
#include "sqlite3.h"

#include <algorithm>
#include <execution>
#include <mutex>
#include <random>
#include <vector>

namespace Event {

    /// @brief Linking subclass used to link people to treatment
    class ScreenageLinking : public Event {
    private:
        /// @brief Supporting function called from execute. Use to establish
        /// links
        /// @param person Individual Person working through the event
        void doEvent(std::shared_ptr<Person::Person> person) override;

    public:
        ScreenageLinking(){};
        virtual ~ScreenageLinking() = default;
    };

} // namespace Event

#endif
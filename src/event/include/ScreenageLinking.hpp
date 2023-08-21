#ifndef EVENT_SCREENAGELINKING_HPP_
#define EVENT_SCREENAGELINKING_HPP_


#include "Event.hpp"
#include "sqlite3.h"

#include <random>
#include <vector>
#include <mutex>
#include <algorithm>
#include <execution>

namespace Event{

/// @brief Linking subclass used to link people to treatment
class ScreenageLinking : public Event{
private:
    /// @brief Supporting function called from execute. Use to establish links
    /// @param person Individual Person working through the event
    void doEvent(Person::Person &person) override;

public:
    ScreenageLinking() {};
    virtual ~ScreenageLinking() = default;
};

}

#endif
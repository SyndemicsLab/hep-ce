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
    void link(Person::Person &person);
    int timestep = -1;

public:
    ScreenageLinking() : Event() {};
    virtual ~ScreenageLinking() = default;
    
    /// @brief Implementation of abstract function, primary entry point for all events
    /// @param population vector containing the people undergoing the simulation
    /// @param timestep current timestep of the simulation
    /// @return vector of people after the event occurs
    void execute(std::vector<Person::Person> &population, int timestep) override;
};

}

#endif
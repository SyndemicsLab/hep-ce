#ifndef EVENT_LINKING_HPP_
#define EVENT_LINKING_HPP_


#include "Event.hpp"
#include "sqlite3.h"

#include <random>
#include <vector>
#include <mutex>
#include <algorithm>
#include <execution>

namespace Event{

class Linking : public Event{
private:
    void link(Person::Person &person);

public:
    Linking() : Event() {};
    virtual ~Linking() = default;

    std::vector<Person::Person> execute(std::vector<Person::Person> population);
};

}

#endif
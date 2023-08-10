#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"

#include <random>
#include <mutex>

namespace Event{

class Screening : public Event {
private:
    uint64_t backgroundSeed;
    std::mt19937_64 generator;
    std::mutex generatorMutex;


    void screen(Person::Person &person);
public:
    Screening() : Event() { this->Screening((uint64)0); };
    Screening(uint64_t seed);
    virtual ~Screening() = default;

    std::vector<Person::Person> execute(std::vector<Person::Person> population);
};

}

#endif
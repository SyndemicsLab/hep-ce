#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"

#include <random>

namespace Event{

class Screening : public Event {
private:
    uint64_t backgroundSeed;
    std::mt19937_64 generator;
public:
    Screening() : Event() {};
    virtual ~Screening() = default;

    virtual bool checkIfScreen(std::vector<Person::Person> population) = 0;

    std::vector<Person::Person> execute(std::vector<Person::Person> population){
        if(!this->checkIfScreen(population)) { return population; }
        return std::vector<Person::Person>();
    };
};

class OneTimeScreening : public Screening {
public:
    OneTimeScreening() : Screening() {};
    virtual ~OneTimeScreening() = default;

    bool checkIfScreen(std::vector<Person::Person> population);
};

class PeriodicScreening : public Screening {
public:
    PeriodicScreening() : Screening() {};
    virtual ~PeriodicScreening() = default;

    bool checkIfScreen(std::vector<Person::Person> population);
};

class BackgroundScreening : public Screening {
public:
    BackgroundScreening() : Screening() {};
    virtual ~BackgroundScreening() = default;

    bool checkIfScreen(std::vector<Person::Person> population);
};

class InterventionScreening : public Screening {
public:
    InterventionScreening() : Screening() {};
    virtual ~InterventionScreening() = default;

    bool checkIfScreen(std::vector<Person::Person> population);
};

}

#endif
#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"
#include "sqlite3.h"

#include <random>
#include <vector>
#include <mutex>
#include <algorithm>
#include <execution>

namespace Event{

class Screening : public Event {
private:
    uint64_t generatorSeed;
    std::mt19937_64 generator;
    std::mutex generatorMutex;
    std::vector<double> backgroundProbability;
    std::vector<double> interventionProbability;
    std::vector<double> acceptTestProbability;
    sqlite3 *db;



    void screen(Person::Person &person);
    void backgroundScreen(Person::Person &person);
    void interventionScreen(Person::Person & person);
    bool antibodyTest(Person::Person &person);
    bool rnaTest(Person::Person &person);
public:
    Screening() : Event() { Screening((uint64_t)0); };
    Screening(uint64_t seed);
    virtual ~Screening() = default;

    std::vector<Person::Person> execute(std::vector<Person::Person> population);
};

}

#endif
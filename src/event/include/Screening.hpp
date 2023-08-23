#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"
#include "sqlite3.h"

#include <algorithm>
#include <execution>
#include <mutex>
#include <random>
#include <vector>

namespace Event {

    class Screening : public Event {
    private:
        std::mt19937_64 &generator;
        std::mutex generatorMutex;
        std::vector<double> backgroundProbability;
        std::vector<double> interventionProbability;
        std::vector<double> acceptTestProbability;
        sqlite3 *db;

        /// @brief
        /// @param person
        void doEvent(Person::Person &person) override;

        /// @brief
        /// @param person
        void backgroundScreen(Person::Person &person);

        /// @brief
        /// @param person
        void interventionScreen(Person::Person &person);

        /// @brief
        /// @param person
        /// @return
        bool antibodyTest(Person::Person &person);

        /// @brief
        /// @param person
        /// @return
        bool rnaTest(Person::Person &person);

    public:
        Screening(std::mt19937_64 &generator);
        virtual ~Screening() = default;
    };

} // namespace Event

#endif
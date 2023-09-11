#ifndef EVENT_SCREENING_HPP_
#define EVENT_SCREENING_HPP_

#include "Event.hpp"
#include "SQLite3.hpp"

#include <algorithm>
#include <execution>
#include <mutex>
#include <random>
#include <vector>

namespace Event {
    class Screening : public ProbEvent {
    private:
        std::vector<double> backgroundProbability;
        std::vector<double> interventionProbability;
        std::vector<double> acceptTestProbability;

        /// @brief
        /// @param person
        void doEvent(std::shared_ptr<Person::Person> person) override;

        /// @brief
        /// @param person
        void backgroundScreen(std::shared_ptr<Person::Person> person);

        /// @brief
        /// @param person
        void interventionScreen(std::shared_ptr<Person::Person> person);

        /// @brief
        /// @param person
        /// @return
        bool antibodyTest(std::shared_ptr<Person::Person> person);

        /// @brief
        /// @param person
        /// @return
        bool rnaTest(std::shared_ptr<Person::Person> person);

    public:
        Screening(std::mt19937_64 &generator, Data::Database &database);
        virtual ~Screening() = default;
    };

} // namespace Event

#endif

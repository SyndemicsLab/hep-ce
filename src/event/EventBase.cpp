#include "Event.hpp"
#include "Person.hpp"
#include <mutex>
#include <omp.h>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace event {
    /// @brief Abstract class that superclasses all Events. Contains execute
    /// function definition
    class EventBase::Event {
    private:
        virtual void doEvent(person::PersonBase &person) = 0;

    protected:
        std::string QUERY;

    public:
        const std::string EVENT_NAME;

        Event(std::string dataquery, std::string name = std::string("Event"))
            : QUERY(dataquery), EVENT_NAME(name) {}
        virtual ~Event() = default;

        /// @brief Primary function required in all Event subclasses. Used to
        /// run the event
        /// @param  Population vector containing all the people in the
        /// simulation
        /// @param  timestep integer containing the current timestep of the
        /// simulation
        /// @return The population vector after the event is executed
        int execute(person::PersonBase &person) {
            // #pragma omp parallel for
            //             for (int i = 0; i < population.size(); ++i) {
            //                 if (population[i].getIsAlive()) {
            //                     this->doEvent(population[i]);
            //                 }
            //             }
            if (person.IsAlive()) {
                this->doEvent(person);
            }
            return 0;
        }
    };

    /// @brief Abstract class for use with Events that involve sampling from the
    /// random number generator to make decisions.
    class EventBase::ProbEvent : public EventBase::Event {
    protected:
        std::shared_ptr<std::mt19937_64> generator;
        std::mutex generatorMutex;

        /// @brief When making a decision with two or more choices, pick one
        /// based on the provided weight(s).
        /// @details The weights specified in the argument \code{probs} must sum
        /// to no greater than 1.0. In the case that probabilities sum to 1,
        /// the return value is always an index of the vector (i.e. the return
        /// is always < \code{probs.size()}). If the sum of the probabilities is
        /// less than 1.0, then the difference (\code{1.0 - sum(probs)}) is
        /// the probability to draw return value \code{probs.size()}.
        /// @param probs A vector containing the weights of each option.
        /// @return Integer representing the chosen state.
        int getDecision(std::vector<double> probs) {
            if (std::accumulate(probs.begin(), probs.end(), 0.0) > 1.00001) {
                const std::string message =
                    '[' + this->EVENT_NAME + "] " +
                    "Error: Sum of probabilities exceeds 1!";
                throw std::runtime_error(message);
            }
            std::uniform_real_distribution<double> uniform(0.0, 1.0);
            this->generatorMutex.lock();
            double value = uniform(this->generator);
            this->generatorMutex.unlock();
            double reference = 0.0;
            for (int i = 0; i < probs.size(); ++i) {
                reference += probs[i];
                if (value < reference) {
                    return i;
                }
            }
            return (int)probs.size();
        }

    public:
        ProbEvent(std::shared_ptr<std::mt19937_64> generator,
                  std::string dataquery,
                  std::string name = std::string("ProbEvent"))
            : generator(generator), EventBase::Event(dataquery, name) {}
        virtual ~ProbEvent() = default;
    };

    EventBase::EventBase(std::shared_ptr<std::mt19937_64> generator,
                         std::string dataquery, std::string name) {
        // only build a single probability event based on if a generator is
        // included
        if (!generator) {
            pImplPROBEVENT = nullptr;
            pImplEVENT = std::make_unique<Event>(dataquery, name);
        } else {
            pImplEVENT = nullptr;
            pImplPROBEVENT =
                std::make_unique<ProbEvent>(generator, dataquery, name);
        }
    }

    int EventBase::Execute(person::PersonBase &person) {
        if (!pImplPROBEVENT) {
            return pImplPROBEVENT->execute(person);
        } else if (!pImplEVENT) {
            return pImplEVENT->execute(person);
        } else {
            // log error with both pointers as NULL
            return 1;
        }
    }

} // namespace event
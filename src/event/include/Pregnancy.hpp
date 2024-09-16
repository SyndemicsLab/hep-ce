#ifndef EVENT_PREGNANCY_HPP_
#define EVENT_PREGNANCY_HPP_

#include "Event.hpp"
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    /// @brief Subclass of Event used to Assess Pregnancy
    class Pregnancy : public Event {
    private:
        class PregnancyIMPL;
        std::shared_ptr<PregnancyIMPL> impl;
        std::shared_ptr<stats::Decider> decider;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;

    public:
        Pregnancy(std::shared_ptr<stats::Decider> decider,
                  std::shared_ptr<datamanagement::DataManager> dm,
                  std::string name = std::string("Pregnancy"));
        virtual ~Pregnancy() = default;
    };
} // namespace event
#endif // EVENT_PREGNANCY_HPP_
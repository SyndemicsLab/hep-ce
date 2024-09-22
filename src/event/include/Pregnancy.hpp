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
        std::unique_ptr<PregnancyIMPL> impl;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::unique_ptr<stats::Decider> &decider) override;

    public:
        Pregnancy();
        ~Pregnancy();

        // Copy Operations
        Pregnancy(Pregnancy const &) = delete;
        Pregnancy &operator=(Pregnancy const &) = delete;
        Pregnancy(Pregnancy &&) noexcept;
        Pregnancy &operator=(Pregnancy &&) noexcept;
    };
} // namespace event
#endif // EVENT_PREGNANCY_HPP_
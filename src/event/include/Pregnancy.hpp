#ifndef EVENT_PREGNANCY_HPP_
#define EVENT_PREGNANCY_HPP_
#include "Event.hpp"
#include "Utils.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    /// @brief Subclass of Event used to Assess Pregnancy
    class Pregnancy : public ProbEvent {
    private:
        double multDeliveryProb;
        double verticalHCVTransProb;
        double infantHCVTestedProb;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;
        /// @brief Returns the probability of pregnancy
        std::vector<double> getPregnancyProb(person::PersonBase &person);

        std::vector<double> getLiveBirthProb(person::PersonBase &person);

        /// @brief
        /// @return
        std::vector<double> getMiscarriageProb(person::PersonBase &person);

        void checkMiscarriage(person::PersonBase &person);

    public:
        Pregnancy(std::mt19937_64 &generator, Data::IDataTablePtr table,
                  Data::Config &config,
                  std::shared_ptr<spdlog::logger> logger =
                      std::make_shared<spdlog::logger>("default"),
                  std::string name = std::string("Pregnancy"));
        virtual ~Pregnancy() = default;
    };
} // namespace event
#endif // EVENT_PREGNANCY_HPP_
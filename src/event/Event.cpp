#include "Event.hpp"
#include "Person.hpp"

namespace event {
    int Event::Execute(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::shared_ptr<stats::DeciderBase> decider) {
        if (person->IsAlive()) {
            this->doEvent(person, dm, decider);
        }
        return 0;
    }

    double Event::DiscountEventCost(double cost, double discount_rate,
                                    int timestep) {
        // dividing discountRate by 12 because discount rates are annual
        double denominator = std::pow(1 + discount_rate / 12, timestep + 1);
        return cost / denominator;
    }

    Event::Event() {}

    Event::~Event() = default;
    Event::Event(Event &&) noexcept = default;
    Event &Event::operator=(Event &&) noexcept = default;
} // namespace event
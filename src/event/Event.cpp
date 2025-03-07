#include "Event.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"

namespace event {
int Event::Execute(std::shared_ptr<person::PersonBase> person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   std::shared_ptr<stats::DeciderBase> decider) {
    if (person->IsAlive()) {
        if (dm == nullptr) {
            spdlog::get("main")->warn(
                "No Data Manager Provided during Construction. No Data "
                "Loaded to Event.");
            return -1;
        }
        this->DoEvent(person, dm, decider);
    }
    return 0;
}

double Event::DiscountEventCost(double cost, double discount_rate, int timestep,
                                bool annual) {
    // dividing discountRate by 12 because discount rates are annual
    discount_rate = annual ? discount_rate / 12 : discount_rate;
    double denominator = std::pow(1 + discount_rate, timestep);
    return cost / denominator;
}

Event::Event() {}

Event::~Event() = default;
Event::Event(Event &&) noexcept = default;
Event &Event::operator=(Event &&) noexcept = default;
} // namespace event

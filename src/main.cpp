#include "Aging.hpp"
#include "BehaviorChanges.hpp"
#include "Clearance.hpp"
#include "DataTable.hpp"
#include "DiseaseProgression.hpp"
#include "Infections.hpp"
#include "Person.hpp"
#include "Simulation.hpp"
#include <memory>

int main(int argc, char *argv[]) {
    Simulation sim(0, 0);
    Data::DataTable table;

    std::vector<std::shared_ptr<Event::Event>> personEvents;
    std::shared_ptr<Event::Event> aging = std::make_shared<Event::Aging>();
    personEvents.push_back(aging);
    std::shared_ptr<Event::Event> behavior =
        std::make_shared<Event::BehaviorChanges>(sim.getGenerator(), table);
    personEvents.push_back(behavior);
    std::shared_ptr<Event::Event> clearance =
        std::make_shared<Event::Clearance>(sim.getGenerator(), table);
    personEvents.push_back(clearance);
    std::shared_ptr<Event::Event> infection =
        std::make_shared<Event::Infections>(sim.getGenerator(), table);
    personEvents.push_back(infection);
    std::shared_ptr<Event::Event> disease =
        std::make_shared<Event::DiseaseProgression>(sim.getGenerator(), table);
    personEvents.push_back(disease);

    sim.loadEvents(personEvents);

    return 0;
}

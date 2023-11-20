#include "AllEvents.hpp"
#include "DataTable.hpp"
#include "Person.hpp"
#include "Simulation.hpp"
#include <memory>

using sharedEvent = std::shared_ptr<Event::Event>;

template <typename T> sharedEvent makeEvent() { return std::make_shared<T>(); }

template <typename T>
sharedEvent makeEvent(std::mt19937_64 &generator, Data::DataTable &table) {
    return std::make_shared<T>(generator, table);
}

int main(int argc, char *argv[]) {
    Simulation sim(0, 0);
    Data::DataTable table;

    // create the person-level event vector
    std::vector<sharedEvent> personEvents;
    sharedEvent aging = makeEvent<Event::Aging>();
    personEvents.push_back(aging);
    sharedEvent behavior =
        makeEvent<Event::BehaviorChanges>(sim.getGenerator(), table);
    personEvents.push_back(behavior);
    sharedEvent clearance =
        makeEvent<Event::Clearance>(sim.getGenerator(), table);
    personEvents.push_back(clearance);
    sharedEvent disease =
        makeEvent<Event::DiseaseProgression>(sim.getGenerator(), table);
    personEvents.push_back(disease);
    sharedEvent infection =
        makeEvent<Event::Infections>(sim.getGenerator(), table);
    personEvents.push_back(infection);

    sim.loadEvents(personEvents);

    return 0;
}

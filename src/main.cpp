#include "Aging.hpp"
#include "BehaviorChanges.hpp"
#include "Clearance.hpp"
#include "DiseaseProgression.hpp"
#include "Infections.hpp"
#include "Person.hpp"
#include "SQLite3.hpp"
#include "Simulation.hpp"
#include <memory>

int main(int argc, char *argv[]) {
    Simulation sim(0, 0);
    Data::Database db("HEP-CE.db");

    std::vector<std::shared_ptr<Event::Event>> person;
    std::shared_ptr<Event::Event> aging =
        std::make_shared<Event::Aging>();
    person.push_back(aging);
    std::shared_ptr<Event::Event> behavior =
        std::make_shared<Event::BehaviorChanges>(sim.getGenerator(), db);
    person.push_back(behavior);
    std::shared_ptr<Event::Event> disease =
        std::make_shared<Event::DiseaseProgression>(sim.getGenerator(), db);
    person.push_back(disease);
    std::shared_ptr<Event::Event> clearance =
        std::make_shared<Event::Clearance>(sim.getGenerator(), db);
    person.push_back(clearance);
    std::shared_ptr<Event::Event> infection =
        std::make_shared<Event::Infections>(sim.getGenerator(), db);
    person.push_back(infection);

    sim.loadEvents(person);

    return 0;
}

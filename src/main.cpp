#include "BehaviorChanges.hpp"
#include "Person.hpp"
#include "SQLite3.hpp"
#include "Simulation.hpp"

int main(int argc, char *argv[]) {
    Simulation sim(0, 0);
    Data::Database db("HEP-CE.db");
    Event::BehaviorChanges behavior(sim.getGenerator(), db);
    sim.addEventToBeginning(behavior);
    return 1;
}

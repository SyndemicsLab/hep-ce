#include "BehaviorChanges.hpp"
#include "Person.hpp"
#include "Simulation.hpp"

int main(int argc, char *argv[]) {
    Simulation sim(0, 0);
    Event::BehaviorChanges behavior(sim.getGenerator());
    sim.addEventToBeginning(behavior);
    return 1;
}

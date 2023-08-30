#include "Person.hpp"

namespace Person {
    void Person::die() { this->isAlive = false; }

    void Person::grow() {
        if (this->isAlive) {
            this->age++;
        }
    }

    void Person::infect() {
        if (this->infectionStatus.hepcState != HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        this->infectionStatus.timeSinceHEPCStateChange = 0;
        this->seropositivity = true;

        if (this->infectionStatus.fibState != FibrosisState::NONE) {
            return;
        }
        this->infectionStatus.fibState = FibrosisState::NONE;
        this->infectionStatus.timeSinceFibStateChange = 0;
    }

    void Person::updateBehavior() {
        if (this->behaviorState->everUsedDrugs) {
            // person is either in an active or former opioid use state
        } else {
            // person has never used drugs
        }
    }

    FibrosisState Person::diagnoseFibrosis(int timestep) {
        // need to add functionality here
        this->infectionStatus.fibState = FibrosisState::F0;
        return this->infectionStatus.fibState;
    }

    HEPCState Person::diagnoseHEPC(int timestep) {
        // need to add functionality here
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        return this->infectionStatus.hepcState;
    }
} // namespace Person

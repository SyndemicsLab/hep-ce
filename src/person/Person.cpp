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
        if (this->behaviorState == BehaviorState::NEVER) {
            this->behaviorState = BehaviorState::CURRENT;
        } else if (this->behaviorState == BehaviorState::CURRENT) {
            this->behaviorState = BehaviorState::FORMER;
        } else {
            this->behaviorState = BehaviorState::CURRENT;
        }
    }

    FibrosisState Person::diagnoseFibrosis(int timestep) {
        // need to add functionality here
        return FibrosisState::F0;
    }

    HEPCState Person::diagnoseHEPC(int timestep) {
        // need to add functionality here
        return HEPCState::ACUTE;
    }
} // namespace Person

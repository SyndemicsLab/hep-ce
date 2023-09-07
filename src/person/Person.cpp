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

        if (this->infectionStatus.liverState != LiverState::NONE) {
            return;
        }
        this->infectionStatus.liverState = LiverState::NONE;
        this->infectionStatus.timeSinceLiverStateChange = 0;
    }

    void Person::updateLiver(const LiverState &ls) {
        // nothing to do -- can only advance liver state
        if (ls <= this->infectionStatus.liverState) {
            return;
        }
        this->infectionStatus.liverState = ls;
        this->infectionStatus.timeSinceLiverStateChange = 0;
    }

    void Person::updateBehavior(const BehaviorClassification &bc) {
        // nothing to do
        if (bc == this->behaviorClassification) {
            return;
        }
        // update the behavior classification
        this->behaviorClassification = bc;
    }

    LiverState Person::diagnoseLiver(int timestep) {
        // need to add functionality here
        this->infectionStatus.liverState = LiverState::F0;
        return this->infectionStatus.liverState;
    }

    HEPCState Person::diagnoseHEPC(int timestep) {
        // need to add functionality here
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        return this->infectionStatus.hepcState;
    }
} // namespace Person

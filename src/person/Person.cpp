#include "Person.hpp"

namespace Person {
    int count = 0;

    void Person::die() { this->isAlive = false; }

    void Person::grow() {
        if (this->isAlive) {
            this->age++;
        }
    }

    void Person::infect() {
        // cannot be multiply infected
        if (this->infectionStatus.hepcState != HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        this->infectionStatus.timeSinceHEPCStateChange = 0;
        this->seropositivity = true;

        if (this->infectionStatus.liverState != LiverState::NONE) {
            return;
        }
        // once infected, immediately enter F0
        this->infectionStatus.liverState = LiverState::F0;
        this->infectionStatus.timeSinceLiverStateChange = 0;
    }

    void Person::clearHCV() {
        // cannot clear if the person is not infected
        if (this->infectionStatus.hepcState == HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::NONE;
        this->infectionStatus.timeSinceHEPCStateChange = 0;
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
        // nothing to do -- cannot go back to NEVER
        if (bc == this->behaviorClassification ||
            bc == BehaviorClassification::NEVER) {
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

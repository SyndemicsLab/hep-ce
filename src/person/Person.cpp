#include "Person.hpp"

namespace Person {
    Person::Person() {
        this->behaviorState = {false, false, false};
        this->classifyBehavior();
    }

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

    void Person::updateBehavior(const BehaviorClassification bc) {
        if (bc == this->behaviorClassification) {
            return;
        }
        switch (bc) {
        case BehaviorClassification::NEVER:
            // cannot transition to NEVER
            return;
        case BehaviorClassification::INJECTION:
            this->behaviorState.activeDrugUse = true;
            this->behaviorState.injectionDrugUse = true;
            break;
        case BehaviorClassification::NONINJECTION:
            this->behaviorState.activeDrugUse = true;
            this->behaviorState.injectionDrugUse = false;
            break;
        case BehaviorClassification::FORMER:
            this->behaviorState.activeDrugUse = false;
            break;
        }
        // people only make it to this if they have used drugs
        if (!this->behaviorState.everUsedDrugs) {
            this->behaviorState.everUsedDrugs = true;
        }
        // update the behavior classification
        this->behaviorClassification = bc;
    }

    void Person::classifyBehavior() {
        if (this->behaviorState.everUsedDrugs) {
            if (this->behaviorState.activeDrugUse) {
                if (this->behaviorState.injectionDrugUse) {
                    this->behaviorClassification =
                        BehaviorClassification::INJECTION;
                } else {
                    this->behaviorClassification =
                        BehaviorClassification::NONINJECTION;
                }
            } else {
                this->behaviorClassification = BehaviorClassification::FORMER;
            }
        } else {
            this->behaviorClassification = BehaviorClassification::NEVER;
        }
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

#include "Person.hpp"

namespace Person {
    int count = 0;

    Person::Person(std::vector<std::string> dataTableRow, int simCycle) {
        count++;
        if (dataTableRow.empty() || dataTableRow.size() < 28) {
            return;
        }

        this->id = stoi(dataTableRow[0]);

        std::transform(dataTableRow[1].begin(), dataTableRow[1].end(),
                       dataTableRow[1].begin(), ::tolower);

        this->sex =
            (dataTableRow[1] == std::string("male")) ? Sex::MALE : Sex::FEMALE;
        // age, gender, idu, infection, fibrosis,
    }

    void Person::die() { this->isAlive = false; }

    void Person::grow() {
        if (this->isAlive) {
            this->age++;
        }
    }

    void Person::infect(int timestep) {
        // cannot be multiply infected
        if (this->infectionStatus.hepcState != HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        this->infectionStatus.timeHEPCStateChanged = timestep;
        this->infectionStatus.seropositivity = true;

        if (this->infectionStatus.liverState != LiverState::NONE) {
            return;
        }
        // once infected, immediately enter F0
        this->infectionStatus.liverState = LiverState::F0;
        this->infectionStatus.timeLiverStateChanged = timestep;
    }

    void Person::clearHCV(int timestep) {
        // cannot clear if the person is not infected
        if (this->infectionStatus.hepcState == HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::NONE;
        this->infectionStatus.timeHEPCStateChanged = timestep;
    }

    void Person::updateLiver(const LiverState &ls, int timestep) {
        // nothing to do -- can only advance liver state
        if (ls <= this->infectionStatus.liverState) {
            return;
        }
        this->infectionStatus.liverState = ls;
        this->infectionStatus.timeLiverStateChanged = timestep;
    }

    void Person::updateBehavior(const BehaviorClassification &bc,
                                int timestep) {
        // nothing to do -- cannot go back to NEVER
        if (bc == this->behaviorDetails.behaviorClassification ||
            bc == BehaviorClassification::NEVER) {
            return;
        }
        // count for timeSinceActive if switching from active to non-active use
        if ((bc == BehaviorClassification::FORMER_NONINJECTION) ||
            (bc == BehaviorClassification::FORMER_INJECTION)) {
            this->behaviorDetails.timeLastActive = timestep;
        }
        // update the behavior classification
        this->behaviorDetails.behaviorClassification = bc;
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

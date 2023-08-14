#include "Person.hpp"

namespace Person{
    void Person::die(){
        this->isAlive = false;
    }

    void Person::grow(){
        if (this->isAlive){
            this->age++;
        }
    }

    void Person::infect(){
        if (this->hepceState == HEPCState::NONE){
            this->hepceState = HEPCState::ACUTE;
        }
        else if (this->hepceState == HEPCState::ACUTE){
            this->hepceState = HEPCState::CHRONIC;
        }
        else{
            // Already Chronic, so do nothing
        }
    }

    void Person::updateBehavior(){
        if (this->behaviorState == BehaviorState::NEVER){
            this->behaviorState = BehaviorState::CURRENT;
        }
        else if (this->behaviorState == BehaviorState::CURRENT){
            this->behaviorState = BehaviorState::FORMER;
        }
        else{
            this->behaviorState = BehaviorState::CURRENT;
        }
    }

    FibrosisState Person::diagnoseFibrosis(){
        // need to add functionality here
        return FibrosisState::F0;
    }

    HEPCState Person::diagnoseHEPC(){
        // need to add functionality here
        return HEPCState::ACUTE;
    }

}
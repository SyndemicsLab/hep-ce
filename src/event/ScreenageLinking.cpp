#include "ScreenageLinking.hpp"

namespace Event{
    void ScreenageLinking::doEvent(Person::Person &person){
        Person::HEPCState state = person.getHEPCState();
        if (state == Person::HEPCState::NONE){
            // add false positive cost
            person.unlink(this->getCurrentTimestep());
            return;
        }

        if (!person.isIdentifiedAsInfected()){
            person.identifyAsInfected(this->getCurrentTimestep());
        }

        if (person.getLinkageType() == Person::LinkageType::BACKGROUND){
            // link probability
        }
        else{
            // add intervention cost
            // link probability
        }

        if (person.getLinkState() == Person::LinkageState::UNLINKED){
            // scale by relink multiplier
        }

        // draw from link probability
        bool value = false;

        if (value){
            // need to figure out how to pass in the LinkageType to the event
            person.link(this->getCurrentTimestep(), Person::LinkageType::BACKGROUND); 
        }
        else if (!value && person.getLinkState() == Person::LinkageState::LINKED){
            person.unlink(this->getCurrentTimestep());
        }
    }

}
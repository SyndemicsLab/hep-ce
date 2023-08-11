#include "Linking.hpp"

namespace Event{

    std::vector<Person::Person> Linking::execute(std::vector<Person::Person> population, int timestep){
        this->timestep = timestep;
        std::for_each(std::execution::par, std::begin(population), std::end(population), link);
        return population;
    }

    void Linking::link(Person::Person &person){
        Person::HEPCState state = person.getHEPCState();
        if (state == Person::HEPCState::NONE){
            // add false positive cost
            person.unlink(this->timestep);
            return;
        }

        if (!person.isIdentifiedAsInfected()){
            person.identifyAsInfected(this->timestep);
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
            person.link(this->timestep, Person::LinkageType::BACKGROUND); 
        }
        else if (!value && person.getLinkState() == Person::LinkageState::LINKED){
            person.unlink(this->timestep);
        }
    }

}
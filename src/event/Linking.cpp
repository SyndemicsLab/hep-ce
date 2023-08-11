#include "Linking.hpp"

namespace Event{

    std::vector<Person::Person> Linking::execute(std::vector<Person::Person> population){
        std::for_each(std::execution::par, std::begin(population), std::end(population), link);
        return population;
    }

    void Linking::link(Person::Person &person){
        
    }

}
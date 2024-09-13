#include "run.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

int loadPopulation(std::vector<sharedPerson> &population,
                   std::unordered_map<std::string, Data::IDataTablePtr> &tables,
                   Simulation::Simulation &sim) {
    if (tables.find("population") != tables.end()) {
        for (int rowIdx = 0;
             rowIdx < tables["population"]->getShape().getNRows(); ++rowIdx) {
            population.push_back(
                makePerson(tables["population"]->getRow(rowIdx)));
        }
    }
    return 0;
}

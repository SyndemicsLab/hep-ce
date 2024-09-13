#ifndef RUN_HPP_
#define RUN_HPP_

#include "Simulation.hpp"
// #include "spdlog/sinks/basic_file_sink.h"
#include "EventFactory.hpp"
#include "spdlog/spdlog.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/// @brief
/// @param personEvents
/// @param dirpath
void writeEvents(std::vector<Event::sharedEvent> &personEvents,
                 std::string dirpath);

/// @brief
/// @param tables
int loadTables(std::unordered_map<std::string, Data::IDataTablePtr> &tables,
               std::string dirpath);

/// @brief
/// @param population
/// @param tables
/// @param sim
int loadPopulation(std::vector<sharedPerson> &population,
                   std::unordered_map<std::string, Data::IDataTablePtr> &tables,
                   Simulation::Simulation &sim);

Data::IDataTablePtr personToDataTable(sharedPerson &person);

void writePopulation(std::vector<sharedPerson> &population,
                     std::string dirpath);
#endif

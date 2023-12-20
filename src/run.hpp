#ifndef RUN_HPP_
#define RUN_HPP_

#include "AllEvents.hpp"
#include "Configuration.hpp"
#include "DataTable.hpp"
#include "Simulation.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <filesystem>
#include <memory>
#include <unordered_map>

/// @brief
using sharedEvent = std::shared_ptr<Event::Event>;

/// @brief
using sharedPerson = std::shared_ptr<Person::Person>;

/// @brief
/// @tparam T
/// @return
template <typename T>
sharedEvent makeEvent(Data::DataTable &table, Data::Configuration &config) {
    return std::make_shared<T>(table, config);
}

/// @brief
/// @tparam T
/// @param generator
/// @param table
/// @return
template <typename T>
sharedEvent makeEvent(std::mt19937_64 &generator, Data::DataTable &table,
                      Data::Configuration &config) {
    return std::make_shared<T>(generator, table, config);
}

/// @brief
/// @tparam T
/// @return
template <typename T> sharedPerson makePerson() {
    return std::make_shared<T>();
}

/// @brief
/// @tparam T
/// @return
template <typename T>
sharedPerson makePerson(Data::DataTable rowData, int simCycle) {
    return std::make_shared<T>(rowData, simCycle);
}

/// @brief
/// @param argc
/// @param argv
/// @param rootInputDir
/// @param taskStart
/// @param taskEnd
/// @return
bool argChecks(int argc, char **argv, std::string &rootInputDir, int &taskStart,
               int &taskEnd);

/// @brief
/// @param personEvents
/// @param tables
/// @param sim
void loadEvents(std::vector<sharedEvent> &personEvents,
                std::unordered_map<std::string, Data::DataTable> &tables,
                Simulation::Simulation &sim, Data::Configuration &config);

/// @brief
/// @param personEvents
/// @param dirpath
void writeEvents(std::vector<sharedEvent> &personEvents, std::string dirpath);

/// @brief
/// @param tables
void loadTables(std::unordered_map<std::string, Data::DataTable> &tables,
                std::string dirpath);

/// @brief
/// @param population
/// @param tables
/// @param sim
void loadPopulation(std::vector<sharedPerson> &population,
                    std::unordered_map<std::string, Data::DataTable> &tables,
                    Simulation::Simulation &sim);

Data::DataTable personToDataTable(sharedPerson &person);

void writePopulation(std::vector<sharedPerson> &population,
                     std::string dirpath);

inline std::string const boolToString(bool b) {
    return b ? std::string("true") : std::string("false");
}

#endif
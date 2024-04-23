#ifndef RUN_HPP_
#define RUN_HPP_

#include "EventFactory.hpp"
#include "Simulation.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <DataManagement.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/// @brief
using sharedPerson = std::shared_ptr<Person::Person>;

/// @brief
/// @tparam T
/// @return
template <typename T> sharedPerson makePerson() {
    return std::make_shared<T>();
}

/// @brief
/// @tparam T
/// @return
template <typename T> sharedPerson makePerson(Data::IDataTablePtr rowData) {
    return std::make_shared<T>(rowData);
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
/// @param config
/// @return
bool configChecks(Data::Config &config);

/// @brief Provide the pRNG seed passed to the simulation object
/// @return User-defined seed, if provided. Otherwise, a seed based on the
/// current millisecond.
uint64_t getSimSeed();

/// @brief
/// @param personEvents
/// @param tables
/// @param sim
int loadEvents(std::vector<Event::sharedEvent> &personEvents,
               std::unordered_map<std::string, Data::IDataTablePtr> &tables,
               Simulation::Simulation &sim, Data::Config &config,
               std::shared_ptr<spdlog::logger> logger =
                   std::make_shared<spdlog::logger>("default"));

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

inline std::string const boolToString(bool b) {
    return b ? std::string("true") : std::string("false");
}
#endif

#include "AllEvents.hpp"
#include "DataTable.hpp"
#include "Simulation.hpp"
#include <memory>
#include <unordered_map>

/// @brief
using sharedEvent = std::shared_ptr<Event::Event>;

/// @brief
using sharedPerson = std::shared_ptr<Person::Person>;

/// @brief
/// @param argc
/// @param argv
/// @param rootInputDir
/// @param taskStart
/// @param taskEnd
/// @return
bool argChecks(int argc, char **argv, std::string &rootInputDir, int &taskStart,
               int &taskEnd) {
    if (argc > 1 && argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                  << "HEP-CE, a microsimulation studying individuals with HCV";
        return false;
    }

    if (argc == 1) {
        std::cout << "Please provide the input folder path: ";
        std::cin >> rootInputDir;
        std::cout << std::endl
                  << "Please provide the first input folder number: ";
        std::cin >> taskStart;
        std::cout << std::endl
                  << "Please provide the last input folder number: ";
        std::cin >> taskEnd;
    } else {
        taskStart = std::stoi(argv[2]);
        taskEnd = std::stoi(argv[3]);
        rootInputDir = argv[1];
    }
    return true;
}

/// @brief
/// @tparam T
/// @return
template <typename T> sharedEvent makeEvent() { return std::make_shared<T>(); }

/// @brief
/// @tparam T
/// @param generator
/// @param table
/// @return
template <typename T>
sharedEvent makeEvent(std::mt19937_64 &generator, Data::DataTable &table) {
    return std::make_shared<T>(generator, table);
}

/// @brief
/// @tparam T
/// @return
template <typename T> sharedPerson makePerson() {
    return std::make_shared<T>();
}

/// @brief
/// @param personEvents
/// @param tables
/// @param sim
void loadEvents(std::vector<sharedEvent> &personEvents,
                std::unordered_map<std::string, Data::DataTable> &tables,
                Simulation::Simulation &sim) {

    sharedEvent aging = makeEvent<Event::Aging>();
    sharedEvent overdose = makeEvent<Event::Overdose>();
    sharedEvent death = makeEvent<Event::Death>();
    sharedEvent behavior = makeEvent<Event::BehaviorChanges>(
        sim.getGenerator(), tables["behavior"]);
    sharedEvent clearance =
        makeEvent<Event::Clearance>(sim.getGenerator(), tables["clearance"]);
    sharedEvent disease = makeEvent<Event::DiseaseProgression>(
        sim.getGenerator(), tables["disease"]);
    sharedEvent infection =
        makeEvent<Event::Infections>(sim.getGenerator(), tables["infection"]);
    sharedEvent voluntaryRelink = makeEvent<Event::VoluntaryRelinking>();
    sharedEvent backgroundScreen = makeEvent<Event::Screening>();
    sharedEvent linking = makeEvent<Event::ScreenageLinking>();
    sharedEvent fibrosis = makeEvent<Event::Fibrosis>();
    sharedEvent treatment = makeEvent<Event::Treatment>();

    personEvents.insert(personEvents.end(),
                        {aging, overdose, death, behavior, clearance, disease,
                         infection, voluntaryRelink, backgroundScreen, linking,
                         fibrosis, treatment});
}

/// @brief
/// @param tables
void loadTables(std::unordered_map<std::string, Data::DataTable> &tables,
                std::string dirpath) {}
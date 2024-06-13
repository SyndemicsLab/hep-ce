#include "run.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

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

bool configChecks(Data::Config &config);

uint64_t getSimSeed(Data::Config &config) {
    using namespace std::chrono;

    std::shared_ptr<Data::ReturnType> temp =
        config.get_optional("simulation.seed", (int)-1);
    int seed;
    if (temp) {
        seed = std::get<int>(*temp);
    } else {
        seed = -1;
    }
    if (seed == -1) {
        milliseconds ms =
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
        return (uint64_t)ms.count();
    }
    return (uint64_t)seed;
}

int loadEvents(std::vector<Event::sharedEvent> &personEvents,
               std::unordered_map<std::string, Data::IDataTablePtr> &tables,
               Simulation::Simulation &sim, Data::Config &config,
               std::shared_ptr<spdlog::logger> logger) {
    personEvents.clear();

    std::vector<std::string> eventList =
        config.getStringVector("simulation.events");
    Event::EventFactory factory;
    for (std::string eventObj : eventList) {
        Event::sharedEvent ev;
        ev = factory.create(eventObj, tables[eventObj], config, logger,
                            sim.getGenerator());
        if (ev == nullptr) {
            logger->error("{} Event Failed to be Created!", eventObj);
            return -1;
        }
        personEvents.push_back(ev);
        logger->info("{} Event Created", eventObj);
    }
    return 0;
}

void writeEvents(std::vector<Event::sharedEvent> &personEvents,
                 std::string dirpath) {}

int loadTables(std::unordered_map<std::string, Data::IDataTablePtr> &tables,
               std::string dirpath) {

    // Added blank table for events that do not need tabular data
    Data::IDataTablePtr blank;
    tables["blank"] = blank;
    tables["Clearance"] = blank;
    tables["VoluntaryRelinking"] = blank;
    tables["Treatment"] = blank;

    // Costs and Utilities
    std::filesystem::path f =
        ((std::filesystem::path)dirpath) / "antibody_testing.csv";
    Data::IDataTablePtr antibody = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "background_costs.csv";
    Data::IDataTablePtr backgroundCost = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "background_mortality.csv";
    Data::IDataTablePtr backgroundMortality =
        std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "background_utilities.csv";
    Data::IDataTablePtr backgroundUtilities =
        std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "behavior_costs.csv";
    Data::IDataTablePtr behaviorCosts = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "behavior_transitions.csv";
    Data::IDataTablePtr behaviorTransitions =
        std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "behavior_utilities.csv";
    Data::IDataTablePtr behaviorUtilities =
        std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "fibrosis_progression.csv";
    Data::IDataTablePtr fibrosisProgression =
        std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "fibrosis.csv";
    Data::IDataTablePtr fibrosis = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "hcv_costs.csv";
    Data::IDataTablePtr hcvCost = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "hcv_utilities.csv";
    Data::IDataTablePtr hcvUtilities = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "incidence.csv";
    Data::IDataTablePtr incidence = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "screening_and_linkage.csv";
    Data::IDataTablePtr screen = std::make_shared<Data::DataTable>(f);

    // f = ((std::filesystem::path)dirpath) / "all_types_overdose.csv";
    // Data::IDataTablePtr overdoses = std::make_shared<Data::DataTable>(f);

    f = ((std::filesystem::path)dirpath) / "SMR.csv";
    Data::IDataTablePtr smr = std::make_shared<Data::DataTable>(f);

    // f = ((std::filesystem::path)dirpath) / "fatal_overdose.csv";
    // Data::IDataTablePtr fatalOverdoses =
    // std::make_shared<Data::DataTable>(f);

    // f = ((std::filesystem::path)dirpath) / "fibrosis_deaths.csv";
    // Data::IDataTablePtr fibrosisDeaths =
    // std::make_shared<Data::DataTable>(f);

    Data::IDataTablePtr death =
        backgroundMortality->innerJoin(smr, "gender", "gender");

    // death = death->innerJoin(fatalOverdoses, "gender", "gender");
    // death = death->innerJoin(fibrosisDeaths, "gender", "gender");

    Data::IDataTablePtr behavior;
    std::vector<std::string> joinCols = {"gender", "drug_behavior"};
    behavior =
        behaviorTransitions->innerJoin(behaviorCosts, joinCols, joinCols);
    behavior = behavior->innerJoin(behaviorUtilities, joinCols, joinCols);

    fibrosisProgression = fibrosisProgression->innerJoin(
        hcvCost, "metavir_stage", "metavir_stage");
    fibrosisProgression = fibrosisProgression->innerJoin(
        hcvUtilities, "metavir_stage", "metavir_stage");

    // people
    f = ((std::filesystem::path)dirpath) / "init_cohort.csv";
    Data::IDataTablePtr population = std::make_shared<Data::DataTable>(f);

    // resultant event tables
    joinCols = {"age_years", "gender", "drug_behavior"};
    tables["Aging"] =
        backgroundCost->innerJoin(backgroundUtilities, joinCols, joinCols);
    tables["BehaviorChanges"] = behavior;
    tables["FibrosisProgression"] = fibrosisProgression;
    tables["FibrosisStaging"] = fibrosis;
    tables["Infections"] = incidence;
    joinCols = {"age_years", "drug_behavior"};
    tables["Screening"] = antibody->innerJoin(screen, joinCols, joinCols);
    tables["Linking"] = screen;
    // tables["Overdose"] = overdoses;
    tables["Death"] = death;
    tables["population"] = population;
    return 0;
}

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

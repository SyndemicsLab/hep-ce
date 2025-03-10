////////////////////////////////////////////////////////////////////////////////
// File: Simulation.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-02                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Simulation.hpp"
#include "DataWriter.hpp"
#include "Decider.hpp"
#include "Event.hpp"
#include "EventFactory.hpp"
#include "Person.hpp"
#include "PersonFactory.hpp"
#include "Utils.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManager.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <random>
#include <sstream>
#include <time.h>

namespace simulation {
class Simulation::SimulationIMPL {
private:
    std::shared_ptr<datamanagement::DataManager> _dm;
    std::shared_ptr<stats::DeciderBase> decider;
    uint64_t _seed;
    int defaultPopulationSize = 0;
    std::vector<std::shared_ptr<person::PersonBase>> population = {};
    std::vector<std::shared_ptr<event::Event>> events = {};
    static std::mt19937_64 generator;

    /// @brief Provide the pRNG seed passed to the simulation object
    /// @return User-defined seed, if provided. Otherwise, a seed based on
    /// the current millisecond.
    uint64_t getSimSeed() {
        using namespace std::chrono;
        milliseconds ms =
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
        std::string data;
        _dm->GetFromConfig("simulation.seed", data);
        uint64_t seed = (data.empty()) ? (uint64_t)ms.count() : std::stoi(data);

        return (uint64_t)seed;
    }

    uint64_t resetSimSeed() {
        using namespace std::chrono;
        milliseconds ms =
            duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
        std::string data;
        _dm->GetFromConfig("simulation.seed", data);
        uint64_t seed = (data.empty()) ? (uint64_t)ms.count() : std::stoi(data);
        _seed = seed;
        spdlog::get("main")->info("Resetting Simulation Seed to {}",
                                  std::to_string(_seed));
        this->generator.seed(_seed);
        this->decider->LoadGenerator(this->generator);
        return (uint64_t)seed;
    }

    int resetPopulationSize() {
        std::string data;
        _dm->GetFromConfig("simulation.population_size", data);
        int size = (data.empty()) ? defaultPopulationSize : std::stoi(data);
        spdlog::get("main")->info("Resetting Population Size to {}",
                                  std::to_string(size));
        defaultPopulationSize = size;
        return 0;
    }

    void LogCycle(int i) {
        if (i % 1000 == 0) {
            spdlog::get("main")->info("person {} completed.", i);
        }
    }

public:
    SimulationIMPL(size_t seed, std::string const &logfile, bool temp_db)
        : _seed(seed) {
        this->generator.seed(_seed);
        this->decider = std::make_shared<stats::Decider>();
        this->decider->LoadGenerator(this->generator);

        std::string logname = (logfile.empty()) ? "logfile.log" : logfile;
        auto file_sink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logname);
        std::vector<spdlog::sink_ptr> sinks{file_sink};

#ifndef NDEBUG
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(console_sink);
#endif

        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));

        spdlog::get("main")->info("Simulation seed: " +
                                  std::to_string(this->_seed));
        _dm = std::make_shared<datamanagement::DataManager>();
        if (temp_db) {
            _dm->ConnectToDatabase("temp.db");
        }
    }

    virtual ~SimulationIMPL() {
        spdlog::get("main")->flush();
        if (std::filesystem::exists("temp.db")) {
            std::filesystem::remove("temp.db");
        }
    }

    int AddCSVTable(std::string const &filepath) {
        return _dm->AddCSVTable(filepath);
    }

    int LoadConfig(std::string const &confpath) {
        int rc = _dm->LoadConfig(confpath);
        std::string data;
        _dm->GetFromConfig("simulation.seed", data);
        resetSimSeed();
        resetPopulationSize();
        return rc;
    }

    int GetPopulationSize() { return defaultPopulationSize; }

    int BuildPersonTable() {
        std::stringstream query;
        query << "CREATE TABLE IF NOT EXISTS population (";
        query << "id, age, sex, drugBehaviorClassification, "
                 "timeLastActiveDrugUse, seropositivity, isGenotypeThree, "
                 "fibrosisState, identifiedAsPositiveInfection, linkageState";
        query << "isAlive, timeInfectionIdentified, "
                 "trueHCVstate, timeHCVStateChanged, "
                 "timeFibrosisStateChanged, timeLastActiveDrugUse, "
                 "timeOfLinkChange, linkageType, timesLinked, "
                 "measuredFibrosisState, "
                 "timeOfLastStaging, timeOfLastScreening, numABTests, "
                 "numRNATests, timesInfected, timesCleared, "
                 "initiatedTreatment, timeOfTreatmentInitiation, "
                 "minUtility, multUtility";
        query << ")";
        datamanagement::Table table;
        return _dm->Create(query.str(), table);
    }

    void CreatePerson(int id, std::vector<std::string> icValues) {
        person::PersonFactory factory;
        std::shared_ptr<person::PersonBase> newperson = factory.create();
        if (!icValues.empty()) {
            newperson->LoadICValues(id, icValues);
        }
        // TODO Fix this so we can read and load population to DB quicker

        // std::stringstream query;
        // query << "INSERT INTO 'population' VALUES (";
        // query << newperson->GetPersonDataString();
        // query << ")";
        // datamanagement::Table table;
        // _dm->Update(query.str(), table);
        this->population.push_back(newperson);
    }

    /// @brief Add an Event to the end of the Event List
    /// @param event Event to add to the Simulation Event List
    int AddEventToEnd(std::shared_ptr<event::Event> &event) {
        spdlog::get("main")->info("Event being added to end of Event Queue");
        this->events.push_back(event);
        return 0;
    }

    /// @brief Add an Event to the beginning of the Event List
    /// @param event Event to add to the Simulation Event List
    int AddEventToBeginning(std::shared_ptr<event::Event> &event) {
        spdlog::get("main")->info(
            "Event being added to beginning of Event Queue.");
        this->events.insert(this->events.begin(), event);
        return 0;
    }

    /// @brief Add an Event to the provided index in the Event List
    /// @param event Even to add to the Simulation Event List
    /// @param idx Index of the location to add the Event
    /// @return True if it succeeds, False if it fails
    int AddEventAtIndex(std::shared_ptr<event::Event> &event, int idx) {
        if (idx >= this->events.size() || idx < 0) {
            spdlog::get("main")->warn(
                "Index {0} out of Event Queue Range of size {1}!",
                std::to_string(idx), std::to_string(this->events.size()));
            return 1;
        }
        spdlog::get("main")->info(
            "Event being added to index {0} of Event Queue.",
            std::to_string(idx));
        this->events.insert(this->events.begin() + idx, event);
        return 0;
    }

    /// @brief Retrieve the Population Vector from the Simulation
    /// @return List of People in the Simulation
    std::vector<std::shared_ptr<person::PersonBase>> getPopulation() const {
        return this->population;
    }

    /// @brief Retrieve the Events in the Simulation
    /// @return List of Events in the Simulation
    std::vector<std::shared_ptr<event::Event>> getEvents() const {
        return this->events;
    }

    /// @brief Execute the Simulation
    /// @return The Final State of the entire Population
    int run() {
        spdlog::get("main")->info("Simulation Run Started");
        std::string data = "";
        if (_dm->GetFromConfig("simulation.duration", data) != 0) {
            spdlog::get("main")->error(
                "Simulation Found no Duration Parameter!\nExiting...");
            exit(-1);
        }
        size_t duration = std::stol(data);

        size_t thread_size = 36;
        // omp_set_dynamic(0);
        // omp_set_num_threads(thread_size);

        std::vector<std::shared_ptr<datamanagement::DataManager>> dm_pool;

        spdlog::get("main")->info("Data Management Pool Creation Started.");
        for (int i = 0; i < thread_size; ++i) {
            std::shared_ptr<datamanagement::DataManager> dm_copy =
                std::make_shared<datamanagement::DataManager>();
            dm_copy->ConnectToDatabase(_dm->GetDBFileName());
            int rc = dm_copy->LoadConfig(_dm->GetConfigFile());
            dm_pool.push_back(dm_copy);
        }

        spdlog::get("main")->info("Data Management Pool Created.");

        // Might need to switch to MPI to make use of multiple
        // nodes instead of just cores:
        // https://stackoverflow.com/questions/52496748/parallel-for-loop-in-c-using-mpi
        clock_t cStartClock = clock();
        spdlog::get("main")->info("Simulation Started.");
        int population_size = population.size();
#pragma omp parallel shared(duration, population_size, events, decider)
        {
#pragma omp for ordered
            for (int i = 0; i < population_size; ++i) {
                std::shared_ptr<person::PersonBase> &p = population[i];
                for (int t = 0; t < duration; ++t) {
                    for (int j = 0; j < events.size(); ++j) {
#pragma omp ordered
                        {
                            std::shared_ptr<event::Event> event = events[j];
                            event->Execute(p, dm_pool[omp_get_thread_num()],
                                           decider);
                        }
                    }
                }
                LogCycle(i);
            }
        }
        spdlog::get("main")->info("Simulation completed in {} seconds",
                                  (clock() - cStartClock) /
                                      ((double)CLOCKS_PER_SEC));
        return 0;
    }

    /// @brief Access the random number generator, for events that need to
    /// sample the pRNG
    /// @return Reference to the simulation's pseudorandom number generator
    std::mt19937_64 &getGenerator() { return generator; }

    int LoadEvents() {
        this->events.clear();
        std::string data;
        _dm->GetFromConfig("simulation.events", data);

        std::vector<std::string> eventList =
            Utils::split2vecT<std::string>(data, ',');

        event::EventFactory factory;
        for (std::string eventObj : eventList) {

            auto ev = factory.create(eventObj, _dm);
            this->events.push_back(ev);
            spdlog::get("main")->info("{} Event Created", eventObj);
        }
        return 0;
    }

    /// @brief Function used to Create Population Set
    /// @param N Number of People to create in the Population
    int CreateNPeople(size_t const N) {
        BuildPersonTable();
        this->population.clear();
        _dm->StartTransaction();
        datamanagement::Table table;
        int rc = _dm->Select("SELECT * FROM init_cohort", table);
        int j = 0;
        for (datamanagement::Row &row : table) {
            if (j < N) {
                CreatePerson(j, row);
            } else {
                break;
            }
            ++j;
        }

        // Fill up Remaining Space to reach N people
        for (size_t i = j; i < N; ++i) {
            CreatePerson(i, {});
        }
        _dm->EndTransaction();
        return 0;
    }

    int WriteResults(std::string const &outfile) {
        writer::DataWriter writer;
        std::filesystem::path popFile =
            std::filesystem::path(outfile) / "population.csv";
        std::string popFileString = popFile.string();
        writer.WritePopulationToFile(this->population, popFileString);
        return 0;
    }

    int SaveSimulationState(std::string const &outfile) {
        return _dm->SaveDatabase(outfile);
    }

    int LoadDirectory(std::string const &dir) {
        int rc = 0;
        if (!std::filesystem::is_directory(dir)) {
            spdlog::get("main")->error("{} is not a valid directory!", dir);
            return 1;
        }
        std::string temp = dir;
        std::string ic_file = "";
        for (const auto &dirEntry :
             std::filesystem::recursive_directory_iterator(temp)) {
            std::filesystem::path p = dirEntry.path();
            if (p.filename() == "init_cohort.csv") {
                ic_file = p;
            } else if (p.extension() == ".csv") {
                // SQLITE_OK == 0, anything evaluating to "True" is SQLite
                // Error
                rc += AddCSVTable(p.string());
            } else if (p.extension() == ".conf") {
                rc += LoadConfig(p.string());
            }
        }
        return rc;
    }

    int LoadDB(std::string const &dbfile) {
        if (_dm == nullptr) {
            _dm = std::make_shared<datamanagement::DataManager>();
        }
        return _dm->ConnectToDatabase(dbfile);
    }
}; // namespace simulation
std::mt19937_64 Simulation::SimulationIMPL::generator;

Simulation::Simulation(size_t seed, std::string const &logfile, bool temp_db) {
    pImplSIM = std::make_unique<SimulationIMPL>(seed, logfile, temp_db);
}
Simulation::Simulation(std::string const &logfile, bool temp_db)
    : Simulation::Simulation(1234, logfile, temp_db) {}
Simulation::~Simulation() {}
int Simulation::Run() { return pImplSIM->run(); }

/// @brief Load an entire directory to the DataManager
/// @param indir Directory containing CSVs and Config file
/// @return 0 if success, positive int otherwise
int Simulation::LoadData(std::string const &s, std::string const &conf,
                         DataType const datatype) {
    int rc = 0;
    rc += LoadConfig(conf);
    switch (datatype) {
    case DataType::CSV:
        rc += pImplSIM->LoadDirectory(s);
        break;
    case DataType::SQL:
        rc += pImplSIM->LoadDB(s);
        break;
    default:
        rc += 1;
    }
    LoadEvents();
    CreateNPeople(pImplSIM->GetPopulationSize());

    return rc;
}

/// @brief Load a Single Table to the Database
/// @param infile file path to the table stored as CSV file
/// @return 0 if success, positive int otherwise
int Simulation::LoadTable(std::string const &infile) {
    if (!std::filesystem::exists(infile)) {
        spdlog::get("main")->warn(
            "File {} not found when attempting to load table!", infile);
        return 1;
    }
    if (pImplSIM->AddCSVTable(infile) != 0) {
        spdlog::get("main")->warn("Failed to add {} to the Database!", infile);
        return 1;
    }
    return 0;
}
int Simulation::LoadConfig(std::string const &infile) {
    if (!std::filesystem::exists(infile)) {
        spdlog::get("main")->warn(
            "File {} not found when attempting to load config!", infile);
        return 1;
    }
    if (pImplSIM->LoadConfig(infile) != 0) {
        spdlog::get("main")->warn("Failed to add config file {}!", infile);
        return 1;
    }
    return 0;
}

int Simulation::LoadEvents() { return pImplSIM->LoadEvents(); }
int Simulation::CreateNPeople(size_t const N) {
    return pImplSIM->CreateNPeople(N);
}

int Simulation::WriteResults(std::string const &outfile) {
    return pImplSIM->WriteResults(outfile);
}
int Simulation::SaveSimulationState(std::string const &outfile) {
    return pImplSIM->SaveSimulationState(outfile);
}
int Simulation::LoadSimulationState(std::string const &infile) { return -1; }

} // namespace simulation

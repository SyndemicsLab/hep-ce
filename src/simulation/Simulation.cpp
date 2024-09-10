//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the Simulation Class.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Simulation.hpp"
#include "EventBase.hpp"
#include "Person.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <DataManager.hpp>
#include <filesystem>

namespace simulation {
    class Environment::Simulation {
    private:
        datamanagement::DataManager _dm;
        int currentTimestep = 0;
        uint64_t _seed;
        std::vector<person::Person> population = {};
        std::vector<event::EventBase> events = {};
        static std::mt19937_64 generator;

    public:
        Simulation(size_t seed = 1234, std::string const &logfile = "")
            : _seed(seed) {
            this->generator.seed(_seed);
            auto console_sink =
                std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                    "logfile.log");
            std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
            auto logger = std::make_shared<spdlog::logger>(
                "main", sinks.begin(), sinks.end());
            spdlog::register_logger(logger);

            spdlog::get("main")->info("Simulation seed: " +
                                      std::to_string(this->_seed));
        }

        virtual ~Simulation() { spdlog::get("main")->flush(); }

        int AddCSVTable(std::string const &filepath) {
            return _dm.AddCSVTable(filepath);
        }

        int LoadConfig(std::string const &confpath) {
            return _dm.LoadConfig(confpath);
        }

        /// @brief Function used to Create Population Set
        /// @param N Number of People to create in the Population
        void createPopulation(const int N) {
            if (N > 0) {
                this->population = {};
            }
            for (int i = 0; i < N; ++i) {
                this->population.push_back(std::make_shared<person::Person>());
            }
        }

        /// @brief Function used to Create Events List
        /// @return Vector of Events to be used in the Simulation
        std::vector<std::shared_ptr<Event::Event>> createEvents() {
            this->loadEvents(std::vector<std::shared_ptr<Event::Event>>());
            return this->getEvents();
        }

        /// @brief Function to Load the Population to the Simulation
        /// @param population Population Set Loaded to Simulation
        void loadPopulation(
            std::vector<std::shared_ptr<person::Person>> &population) {
            if (!this->population.empty()) {
                this->logger->info(
                    "Simulation Population not empty, overwriting "
                    "existing population.");
            }
            this->population = population;
        }

        /// @brief Function used to add a Single Person to the Population in the
        /// Simulation
        /// @param person Person to add to the Population
        void addPerson(person::Person person) {
            this->population.push_back(
                std::make_shared<person::Person>(person));
        }

        /// @brief Function used to load the Events List to the Simulation
        /// @param events Events List to Load to the Simulation
        void loadEvents(std::vector<std::shared_ptr<Event::Event>> events) {
            if (events.empty()) {
                this->logger->warn(
                    "Empty Event being added to simulation event "
                    "list! Make sure this is intentional.");
            }
            if (!this->events.empty()) {
                this->logger->info("Simulation Events not empty, overwriting "
                                   "existing events.");
            }
            this->events = events;
        }

        /// @brief Add an Event to the end of the Event List
        /// @param event Event to add to the Simulation Event List
        void addEventToEnd(std::shared_ptr<Event::Event> event) {
            this->logger->info("Event being added to end of Event Queue");
            this->events.push_back(event);
        }

        /// @brief Add an Event to the beginning of the Event List
        /// @param event Event to add to the Simulation Event List
        void addEventToBeginning(std::shared_ptr<Event::Event> event) {
            this->logger->info(
                "Event being added to beginning of Event Queue.");
            this->events.insert(this->events.begin(), event);
        }

        /// @brief Add an Event to the provided index in the Event List
        /// @param event Even to add to the Simulation Event List
        /// @param idx Index of the location to add the Event
        /// @return True if it succeeds, False if it fails
        bool addEventAtIndex(std::shared_ptr<Event::Event> event, int idx) {
            if (idx >= this->events.size() || idx < 0) {
                this->logger->warn(
                    "Index {0} out of Event Queue Range of size {1}!",
                    std::to_string(idx), std::to_string(this->events.size()));
                return false;
            }
            this->logger->info("Event being added to index {0} of Event Queue.",
                               std::to_string(idx));
            this->events.insert(this->events.begin() + idx, event);
            return true;
        }

        /// @brief Retrieve the Population Vector from the Simulation
        /// @return List of People in the Simulation
        std::vector<std::shared_ptr<person::Person>> getPopulation() const {
            return this->population;
        }

        /// @brief Retrieve the Events in the Simulation
        /// @return List of Events in the Simulation
        std::vector<std::shared_ptr<Event::Event>> getEvents() const {
            return this->events;
        }

        /// @brief Execute the Simulation
        /// @return The Final State of the entire Population
        std::vector<std::shared_ptr<person::Person>> run() {
            spdlog::get("main")->info("Simulation Run Started");
            for (currentTimestep; currentTimestep < ((int)this->duration);
                 ++currentTimestep) {
                for (std::shared_ptr<Event::Event> event : this->events) {
                    event->setCurrentTimestep(currentTimestep);
#pragma omp parallel for
                    for (person::Person person : this->population) {
                        event->execute(person);
                    }
                }
                this->logger->info("Simulation completed timestep {}",
                                   currentTimestep);
            }
            return this->population;
        }

        /// @brief Access the random number generator, for events that need to
        /// sample the pRNG
        /// @return Reference to the simulation's pseudorandom number generator
        std::mt19937_64 &getGenerator() { return generator; }

        /// @brief A getter for the Current Timestep variable
        /// @return currentTimestep as a int
        int getCurrentTimestep() { return this->currentTimestep; }
    };
    std::mt19937_64 Simulation::generator;

    Environment::Environment(size_t seed = 1234,
                             std::string const &logfile = "") {
        pImplSIM = std::make_unique<Simulation>(seed, logfile);
    }
    Environment::~Environment() {}
    int Environment::Run() { pImplSIM->run(); }

    /// @brief Load an entire directory to the DataManager
    /// @param indir Directory containing CSVs and Config file
    /// @return 0 if success, positive int otherwise
    int Environment::LoadData(std::string const &indir) {
        int rc = 0;
        if (!std::filesystem::is_directory(indir)) {
            spdlog::get("main")->error("{} is not a valid directory!", indir);
            rc = 1;
        }
        for (const auto &dirEntry :
             std::filesystem::recursive_directory_iterator(indir)) {
            std::filesystem::path p = dirEntry.path();
            if (p.extension() == ".csv") {
                // SQLITE_OK == 0, anything evaluating to "True" is SQLite Error
                rc += this->LoadData(p.string());
            } else if (p.extension() == ".conf") {
                rc += this->LoadConfig(p.string());
            }
        }
        return rc;
    }

    /// @brief Load a Single Table to the Database
    /// @param infile file path to the table stored as CSV file
    /// @return 0 if success, positive int otherwise
    int Environment::LoadTable(std::string const &infile) {
        if (!std::filesystem::exists(infile)) {
            spdlog::get("main")->warn(
                "File {} not found when attempting to load table!", infile);
            return 1;
        }
        if (pImplSIM->AddCSVTable(infile)) {
            spdlog::get("main")->warn("Failed to add {} to the Database!",
                                      infile);
            return 1;
        }
        return 0;
    }
    int Environment::LoadConfig(std::stirng const &infile) {
        if (!std::filesystem::exists(infile)) {
            spdlog::get("main")->warn(
                "File {} not found when attempting to load config!", infile);
            return 1;
        }
        if (pImplSIM->LoadConfig(infile)) {
            spdlog::get("main")->warn("Failed to add config file {}!",
                                      p.string());
            return 1;
        }
        return 0;
    }

    int Environment::WriteResults(std::string const &outfile) {}
    int Environment::SaveSimulationState(std::string const &outfile) {}
    int Environment::LoadSimulationState(std::string const &infile) {}

} // namespace simulation

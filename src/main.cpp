#include "DataWriter.hpp"
#include "EventFactory.hpp"
#include "run.hpp"
#include <iostream>
#include <string>

/// @brief
/// @param argc
/// @param argv
/// @return
int main(int argc, char *argv[]) {
    int taskStart;
    int taskEnd;
    std::string rootInputDir;
    if (!argChecks(argc, argv, rootInputDir, taskStart, taskEnd)) {
        return 0;
    }

    for (int i = taskStart; i < (taskEnd + 1); ++i) {
        std::unordered_map<std::string, Data::IDataTablePtr> tables;

        std::filesystem::path inputSet = ((std::filesystem::path)rootInputDir) /
                                         ("input" + std::to_string(i));

        // load non-tabular inputs
        std::filesystem::path configPath = inputSet / "sim.conf";
        Data::Config config(configPath.string());

        // define output path
        std::filesystem::path outputSet =
            ((std::filesystem::path)rootInputDir) /
            ("output" + std::to_string(i));

        // initialize logger
        std::shared_ptr<spdlog::logger> logger;
        try {
            logger = spdlog::basic_logger_mt("basic_logger",
                                             outputSet.string() + "/log.txt");
            logger->flush_on(spdlog::level::err);
        } catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log init failed: " << ex.what() << std::endl;
            exit(-1);
        }

        // load tabular inputs
        loadTables(tables, inputSet.string());

        Simulation::Simulation sim(
            getSimSeed(config),
            std::get<int>(config.get("simulation.duration", 0)), logger);

        // create the person-level event vector
        std::vector<Event::sharedEvent> personEvents;

        logger->info("Attempting to Load Events");
        int result = loadEvents(personEvents, tables, sim, config, logger);

        // check if events loaded correctly, exit if failed
        if (result == -1) {
            return -1;
        }

        sim.loadEvents(personEvents);
        logger->info("Events loaded to Simulation");

        logger->info("Attempting to load Population");
        std::vector<sharedPerson> population;
        loadPopulation(population, tables, sim);
        sim.loadPopulation(population);
        logger->info("Population Loaded to Simulation");

        sim.run();

        population = sim.getPopulation();
        personEvents = sim.getEvents();

        logger->info("Writing Events");
        writeEvents(personEvents, outputSet.string());
        logger->info("Writing Population");
        DataWriter::writePopulation(population, outputSet.string());
        DataWriter::writeGeneralStats(population, outputSet.string(), config);
    }

    return 0;
}

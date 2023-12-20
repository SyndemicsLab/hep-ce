#include "run.hpp"

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

    std::vector<int> runs((taskEnd + 1) - taskStart);
    std::iota(std::begin(runs), std::end(runs), taskStart);

    std::for_each(std::execution::par_unseq, std::begin(runs), std::end(runs),
                  [&](int i) {
                      std::unordered_map<std::string, Data::DataTable> tables;

                      std::filesystem::path inputSet =
                          ((std::filesystem::path)rootInputDir) /
                          ("input" + std::to_string(i));

                      std::filesystem::path configPath = inputSet / "sim.conf";

                      Data::Configuration config(configPath.string());

                      std::filesystem::path outputSet =
                          ((std::filesystem::path)rootInputDir) /
                          ("output" + std::to_string(i));

                      std::shared_ptr<spdlog::logger> logger;
                      try {
                          logger = spdlog::basic_logger_mt(
                              "basic_logger", outputSet.string() + "/log.txt");
                      } catch (const spdlog::spdlog_ex &ex) {
                          std::cout << "Log init failed: " << ex.what()
                                    << std::endl;
                          exit(-1);
                      }

                      loadTables(tables, inputSet.string());

                      Simulation::Simulation sim(0, 0, logger);

                      // create the person-level event vector
                      std::vector<sharedEvent> personEvents;
                      loadEvents(personEvents, tables, sim, config, logger);
                      sim.loadEvents(personEvents);

                      std::vector<sharedPerson> population;
                      loadPopulation(population, tables, sim);
                      sim.loadPopulation(population);

                      sim.run();

                      population = sim.getPopulation();
                      personEvents = sim.getEvents();

                      writeEvents(personEvents, outputSet.string());
                      writePopulation(population, outputSet.string());
                  });

    return 0;
}

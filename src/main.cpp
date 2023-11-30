#include "run.cpp"

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
                      Simulation::Simulation sim(0, 0);
                      std::unordered_map<std::string, Data::DataTable> tables;

                      std::filesystem::path inputSet =
                          ((std::filesystem::path)rootInputDir) /
                          ("input" + std::to_string(i));

                      loadTables(tables, inputSet.string());

                      // create the person-level event vector
                      std::vector<sharedEvent> personEvents;
                      loadEvents(personEvents, tables, sim);
                      sim.loadEvents(personEvents);

                      std::vector<sharedPerson> population;
                      loadPopulation(population, tables, sim);
                      sim.loadPopulation(population);
                  });

    return 0;
}

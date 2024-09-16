#include "Simulation.hpp"
#include <filesystem>
#include <iostream>
#include <string>

bool argChecks(int argc, char **argv, std::string &rootInputDir, int &taskStart,
               int &taskEnd);

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
        std::filesystem::path inputSet = ((std::filesystem::path)rootInputDir) /
                                         ("input" + std::to_string(i));
        // define output path
        std::filesystem::path outputSet =
            ((std::filesystem::path)rootInputDir) /
            ("output" + std::to_string(i));
        std::filesystem::path logfile = outputSet / "log.txt";
        simulation::Simulation sim(logfile.string());
        sim.LoadData(inputSet.string());
        sim.LoadEvents();
        sim.CreateNPeople(100);
        sim.Run();
        sim.WriteResults(outputSet.string());
    }

    return 0;
}

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
                  << "HEP-CE, a microsimulation studying individuals "
                     "with HCV";
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

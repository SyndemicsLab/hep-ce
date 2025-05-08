////////////////////////////////////////////////////////////////////////////////
// File: exec.cpp                                                             //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <hepce/event/event.hpp>
#include <hepce/model/person.hpp>
#include <hepce/model/simulation.hpp>

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
        std::filesystem::path dbfile = inputSet / "inputs.db";
        std::filesystem::path cffile = inputSet / "sim.conf";
        std::filesystem::path logfile = outputSet / "log.txt";
        auto sim = hepce::model::Hepce::Create(logfile.string());
        // sim->LoadData(dbfile.string(), cffile.string(),
        //               simulation::DataType::SQL);
        // std::vector<hepce::model::Person> people = {};
        // std::vector<hepce::event::Event> discrete_events = {};
        // int duration = 0;
        // sim->Run(people, discrete_events, 0);
        // sim.WriteResults(outputSet.string());
    }

    return 0;
}

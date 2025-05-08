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

#include <hepce/data/writer.hpp>
#include <hepce/event/event.hpp>
#include <hepce/model/person.hpp>
#include <hepce/model/simulation.hpp>
#include <hepce/utils/logging.hpp>

/// @brief
/// @param argc
/// @param argv
/// @param root_dir
/// @param task_start
/// @param task_end
/// @return
bool argChecks(int argc, char **argv, std::string &root_dir, int &task_start,
               int &task_end) {
    if (argc > 1 && argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                  << "HEP-CE, a microsimulation studying individuals "
                     "with HCV";
        return false;
    }

    if (argc == 1) {
        std::cout << "Please provide the input folder path: ";
        std::cin >> root_dir;
        std::cout << std::endl
                  << "Please provide the first input folder number: ";
        std::cin >> task_start;
        std::cout << std::endl
                  << "Please provide the last input folder number: ";
        std::cin >> task_end;
    } else {
        task_start = std::stoi(argv[2]);
        task_end = std::stoi(argv[3]);
        root_dir = argv[1];
    }
    return true;
}

/// @brief
/// @param argc
/// @param argv
/// @return
int main(int argc, char *argv[]) {
    int task_start;
    int task_end;
    std::string root_dir;
    if (!argChecks(argc, argv, root_dir, task_start, task_end)) {
        return 0;
    }

    for (int i = task_start; i < (task_end + 1); ++i) {
        std::filesystem::path input_dir =
            ((std::filesystem::path)root_dir) / ("input" + std::to_string(i));
        // define output path
        std::filesystem::path output_dir =
            ((std::filesystem::path)root_dir) / ("output" + std::to_string(i));
        std::filesystem::path dbfile = input_dir / "inputs.db";
        std::filesystem::path config = input_dir / "sim.conf";
        std::filesystem::path popfile = output_dir / "population.csv";

        std::filesystem::path log_file = output_dir / "hepce.log";
        std::string log_name = "hepce-task-" + std::to_string(i);
        hepce::utils::CreateFileLogger(log_name, log_file);

        auto model_data = datamanagement::ModelData::Create(config, log_name);
        model_data->AddSource(dbfile);

        auto sim = hepce::model::Hepce::Create(*model_data, log_name);
        auto population = sim->CreatePopulation(*model_data);
        auto events = sim->CreateEvents(*model_data);
        sim->Run(population, events);

        auto writer =
            hepce::data::Writer::Create(output_dir.string(), log_name);
        writer->WritePopulation(population, popfile.string(),
                                hepce::data::OutputType::kFile);
    }

    return 0;
}

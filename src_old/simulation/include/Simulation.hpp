////////////////////////////////////////////////////////////////////////////////
// File: Simulation.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-02                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef SIMULATION_HPP_
#define SIMULATION_HPP_

#include <memory>
#include <string>

// Forward Defining Person to use in Execute
namespace person {
class PersonBase;
}

namespace event {
class EventBase;
}

/// @brief Namespace containing Simulation Level Attributes
namespace simulation {
enum class DataType { CSV = 0, SQL = 1, kCount = 2 };
class Simulation {
private:
    class SimulationIMPL;
    std::unique_ptr<SimulationIMPL> pImplSIM;

public:
    Simulation(size_t seed = 1234, std::string const &logfile = "",
               bool temp_db = true);
    Simulation(std::string const &logfile = "", bool temp_db = true);
    ~Simulation();
    int Run();
    int LoadData(std::string const &infile, std::string const &conf,
                 DataType const datatype = DataType::CSV);
    int LoadTable(std::string const &infile);
    int LoadConfig(std::string const &infile);
    int LoadEvents();
    int CreateNPeople(size_t const N);
    int WriteResults(std::string const &outfile);
    int SaveSimulationState(std::string const &outfile);
    int LoadSimulationState(std::string const &infile);
};

} // namespace simulation
#endif

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
/// This file contains the declaration of the Simulation Class.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
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
enum class DataType { CSV = 0, SQL = 1, COUNT = 2 };
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

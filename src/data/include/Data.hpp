//===-- Data.hpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid 
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Wednesday, August 2nd 2023, 3:16:18 pm
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef DATA_DATA_HPP_
#define DATA_DATA_HPP_

#include <string>

/// @brief Namespace holding Data manipulation classes
namespace Data{

/// @brief Main class for manipulating Data Files. Does not inlcude SQL DBs
class Data {
private:
    std::string dataString;
public:
    Data(){};

    /// @brief Function for Reading Data from the provided CSV
    /// @param filepath string containing the filepath for the csv File
    void read_csv(std::string filepath);

    /// @brief Function for Writing Data to the CSV
    /// @param filepath String containing the filepath to the csv File
    void write_csv(std::string filepath);
};

}
#endif
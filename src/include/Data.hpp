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

#ifndef DATA_HPP_
#define DATA_HPP_

#include <string>

class Data {
private:
    std::string dataString;
public:
    Data(){};
    void read_csv(std::string filepath);
    void write_csv(std::string filepath);
};

#endif
////////////////////////////////////////////////////////////////////////////////
// File: utility.hpp                                                          //
// Project: hep-ce                                                            //
// Created: 2025-08-08                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-08-08                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_TESTS_CONSTANTS_UTILITY_HPP_
#define HEPCE_TESTS_CONSTANTS_UTILITY_HPP_

// STL Includes
#include <filesystem>

// Library Headers
#include <hepce/utils/logging.hpp>

namespace hepce {
namespace testing {
inline void CreateTestLog(std::string log_name) {
    const std::string log_file = log_name + ".log";
    hepce::utils::CreateFileLogger(log_name, log_file);
}

inline void RemoveTestLog(std::string log_name) {
    std::filesystem::remove(log_name + ".log");
}
} // namespace testing
} // namespace hepce
#endif // HEPCE_TESTS_CONSTANTS_UTILITY_HPP_

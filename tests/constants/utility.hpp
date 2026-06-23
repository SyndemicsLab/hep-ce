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
#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <thread>

// Library Headers
#include <hepce/utils/logging.hpp>

// Third-party Includes
#include <spdlog/spdlog.h>

namespace hepce {
namespace testing {
inline void CreateTestLog(std::string log_name) {
    const std::string log_file = log_name + ".log";
    hepce::utils::CreateFileLogger(log_name, log_file);
}

inline void RemoveTestLog(std::string log_name) {
    const std::string log_file = log_name + ".log";

    // Release any open file handles held by the logger before deleting.
    if (auto logger = spdlog::get(log_name); logger) {
        logger->flush();
        spdlog::drop(log_name);
    }

    std::error_code ec;
    for (int attempt = 0; attempt < 10; ++attempt) {
        std::filesystem::remove(log_file, ec);
        if (!ec || !std::filesystem::exists(log_file)) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    throw std::runtime_error("Failed to remove test log file '" + log_file +
                             "': " + ec.message());
}
} // namespace testing
} // namespace hepce
#endif // HEPCE_TESTS_CONSTANTS_UTILITY_HPP_

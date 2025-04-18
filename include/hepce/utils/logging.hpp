////////////////////////////////////////////////////////////////////////////////
// File: logging.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_LOGGING_HPP_
#define HEPCE_UTILS_LOGGING_HPP_

#include <string>

namespace hepce {
namespace utils {
enum class LogType : int { kInfo, kWarn, kError, kDebug, kCount };
enum class CreationStatus : int {
    kError = -1,
    kSuccess = 0,
    kExists = 1,
    kNotCreated = 2,
    kCount = 4
};

CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath);
void LogInfo(const std::string &logger_name, const std::string &message);
void LogWarning(const std::string &logger_name, const std::string &message);
void LogError(const std::string &logger_name, const std::string &message);
void LogDebug(const std::string &logger_name, const std::string &message);

} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_LOGGING_HPP_
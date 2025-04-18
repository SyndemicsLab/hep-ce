////////////////////////////////////////////////////////////////////////////////
// File: logging_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-18                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_LOGGINGINTERNALS_HPP_
#define HEPCE_UTILS_LOGGINGINTERNALS_HPP_

#include <hepce/utils/logging.hpp>

#include <iostream>
#include <string>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace hepce {
namespace utils {
CreationStatus CheckIfExists(const std::string &logger_name) {
    return (spdlog::get(logger_name) != nullptr) ? CreationStatus::kExists
                                                 : CreationStatus::kNotCreated;
}
void log(const std::string &logger_name, const std::string &message,
         LogType type = LogType::kInfo) {
    CreationStatus status = CheckIfExists(logger_name);
    if ((status == CreationStatus::kNotCreated) &&
        (CreateFileLogger(logger_name, "log.txt") == CreationStatus::kError)) {
        std::cerr << "Failed to create logger: " << logger_name << std::endl;
        return;
    }
    auto logger = spdlog::get(logger_name);
    if (logger) {
        switch (type) {
        case LogType::kInfo:
            logger->info(message);
            break;
        case LogType::kWarn:
            logger->warn(message);
            break;
        case LogType::kError:
            logger->error(message);
            break;
        case LogType::kDebug:
            logger->debug(message);
            break;
        default:
            logger->info(message);
            break;
        }
        logger->flush();
    } else {
        spdlog::error("Logger {} not found", logger_name);
    }
}

} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_LOGGINGINTERNALS_HPP_
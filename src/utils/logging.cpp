////////////////////////////////////////////////////////////////////////////////
// File: logging.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-22                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/utils/logging.hpp>

#include "internals/logging_internals.hpp"

#include <sstream>

namespace hepce {
namespace utils {
CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath) {
    if (CheckIfExists(logger_name) == CreationStatus::kExists) {
        return CreationStatus::kExists;
    }
    try {
        spdlog::cfg::load_env_levels();
        std::string pattern =
            LoggingConfig::GetPatternString(LoggingConfig::GetPattern());
        spdlog::set_pattern(pattern);
        spdlog::basic_logger_mt(logger_name, filepath);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
        return CreationStatus::kError;
    }
    return CreationStatus::kSuccess;
}

CreationStatus CreateSharedFileSink(const std::string &filepath) {
    try {
        auto sink = LoggingConfig::GetSharedSink(filepath);
        if (sink) {
            LoggingConfig::SetDefaultSinkPath(filepath);
            return CreationStatus::kSuccess;
        }
        std::string error_msg =
            "Failed to create shared file sink: sink is null";
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    } catch (const spdlog::spdlog_ex &ex) {
        std::string error_msg = "Failed to create shared file sink at '" +
                                filepath + "': " + ex.what();
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    }
}

CreationStatus CreateSharedLogger(const std::string &logger_name) {
    if (CheckIfExists(logger_name) == CreationStatus::kExists) {
        std::cout << "Shared logger " << logger_name << " already exists"
                  << std::endl;
        return CreationStatus::kExists;
    }

    try {
        std::string filepath = LoggingConfig::GetDefaultSinkPath();
        auto sink = LoggingConfig::GetSharedSink(filepath);
        if (!sink) {
            std::string error_msg =
                "Failed to create shared logger '" + logger_name +
                "': could not get or create shared sink at '" + filepath + "'";
            std::cerr << error_msg << std::endl;
            return CreationStatus::kError;
        }

        spdlog::cfg::load_env_levels();
        std::string pattern =
            LoggingConfig::GetPatternString(LoggingConfig::GetPattern());

        auto logger = std::make_shared<spdlog::logger>(logger_name, sink);
        logger->set_pattern(pattern);
        logger->set_level(spdlog::level::trace);

        spdlog::register_logger(logger);

        return CreationStatus::kSuccess;
    } catch (const spdlog::spdlog_ex &ex) {
        std::string error_msg = "Failed to create shared logger '" +
                                logger_name + "': " + ex.what();
        std::cerr << error_msg << std::endl;
        return CreationStatus::kError;
    }
}

void SetLogPattern(LogPattern pattern) { LoggingConfig::SetPattern(pattern); }

LogPattern GetLogPattern() { return LoggingConfig::GetPattern(); }

void SetFlushInterval(int seconds) { LoggingConfig::SetFlushInterval(seconds); }

void FlushAllLoggers() {
    spdlog::apply_all(
        [](std::shared_ptr<spdlog::logger> log) { log->flush(); });
}

void LogInfo(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kInfo);
}

void LogWarning(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kWarn);
}

void LogError(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kError);
}

void LogDebug(const std::string &logger_name, const std::string &message) {
    log(logger_name, message, LogType::kDebug);
}

CreationStatus CheckLoggerExists(const std::string &logger_name) {
    return CheckIfExists(logger_name);
}

std::string GetLoggerInfo(const std::string &logger_name) {
    auto logger = spdlog::get(logger_name);
    if (!logger) {
        return "Logger \"" + logger_name + "\" not found";
    }

    auto level_view = spdlog::level::to_string_view(logger->level());
    std::string info = "Logger: " + logger_name + "\n";
    info +=
        "  Level: " + std::string(level_view.data(), level_view.size()) + "\n";
    info += "  Sinks: " + std::to_string(logger->sinks().size());
    return info;
}

void SetLoggerLevel(const std::string &logger_name, int level) {
    auto logger = spdlog::get(logger_name);
    if (logger) {
        int clamped_level = (level < 0) ? 0 : (level > 5) ? 5 : level;
        logger->set_level(
            static_cast<spdlog::level::level_enum>(clamped_level));
    }
}

std::string ConstructMessage(const std::exception &error, std::string message) {
    std::stringstream msg;
    msg << message << ": " << error.what();
    return msg.str();
}
} // namespace utils
} // namespace hepce

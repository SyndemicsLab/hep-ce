////////////////////////////////////////////////////////////////////////////////
// File: logging_internals.hpp                                                //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-22                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_LOGGINGINTERNALS_HPP_
#define HEPCE_UTILS_LOGGINGINTERNALS_HPP_

#include <hepce/utils/logging.hpp>

#include <iostream>
#include <string>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace hepce {
namespace utils {
class LoggingConfig {
public:
    static LoggingConfig &GetInstance() {
        static LoggingConfig instance;
        return instance;
    }

    static std::shared_ptr<spdlog::sinks::basic_file_sink_mt>
    GetSharedSink(const std::string &filepath) {
        std::lock_guard<std::mutex> lock(GetInstance().sink_mutex_);
        auto key = filepath;
        if (GetInstance().shared_sinks_.find(key) ==
            GetInstance().shared_sinks_.end()) {
            try {
                GetInstance().shared_sinks_[key] =
                    std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                        filepath, false);
            } catch (const spdlog::spdlog_ex &ex) {
                std::cerr << "Failed to create shared sink: " << ex.what()
                          << std::endl;
                return nullptr;
            }
        }
        return GetInstance().shared_sinks_[key];
    }

    static LogPattern GetPattern() { return GetInstance().current_pattern_; }

    static void SetPattern(LogPattern pattern) {
        GetInstance().current_pattern_ = pattern;
    }

    static std::string GetPatternString(LogPattern pattern) {
        switch (pattern) {
        case LogPattern::kSimple:
            return "[%n] %v";
        case LogPattern::kStandard:
            return "[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v";
        case LogPattern::kDetailed:
            return "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%L%$] [thread %t] %v";
        case LogPattern::kThreadSafe:
            return "[%H:%M:%S] [seq %i] [%n] [%^---%L---%$] %v";
        default:
            return "[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v";
        }
    }

    static int GetFlushInterval() { return GetInstance().flush_interval_; }

    static void SetFlushInterval(int seconds) {
        GetInstance().flush_interval_ = seconds;
    }

    static void SetDefaultSinkPath(const std::string &path) {
        std::lock_guard<std::mutex> lock(GetInstance().config_mutex_);
        GetInstance().default_sink_path_ = path;
    }

    static std::string GetDefaultSinkPath() {
        std::lock_guard<std::mutex> lock(GetInstance().config_mutex_);
        return GetInstance().default_sink_path_;
    }

private:
    LoggingConfig()
        : current_pattern_(LogPattern::kStandard), flush_interval_(3),
          default_sink_path_("hepce.log") {
        spdlog::cfg::load_env_levels();
    }

    std::unordered_map<std::string,
                       std::shared_ptr<spdlog::sinks::basic_file_sink_mt>>
        shared_sinks_;
    std::mutex sink_mutex_;
    std::mutex config_mutex_;
    LogPattern current_pattern_;
    int flush_interval_;
    std::string default_sink_path_;
};

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
            logger->set_level(spdlog::level::info);
            logger->info(message);
            break;
        case LogType::kWarn:
            logger->set_level(spdlog::level::warn);
            logger->warn(message);
            break;
        case LogType::kError:
            logger->set_level(spdlog::level::err);
            logger->error(message);
            break;
        case LogType::kDebug:
            logger->set_level(spdlog::level::debug);
            logger->debug(message);
            break;
        default:
            logger->set_level(spdlog::level::info);
            logger->info(message);
            break;
        }
        if (LoggingConfig::GetFlushInterval() == 0) {
            logger->flush();
        }
    } else {
        spdlog::error("Logger {} not found", logger_name);
    }
}

} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_LOGGINGINTERNALS_HPP_
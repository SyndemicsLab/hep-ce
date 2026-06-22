////////////////////////////////////////////////////////////////////////////////
// File: logging.hpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-22                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_LOGGING_HPP_
#define HEPCE_UTILS_LOGGING_HPP_

#include <exception>
#include <string>

namespace hepce {
namespace utils {
/// @brief Logging levels for the logger.
enum class LogType : int {
    kInfo = 0,  // Information Messages
    kWarn = 1,  // Warning Messages
    kError = 2, // Error Messages
    kDebug = 3, // Debug Messages
    kCount = 4  // Enum Counter
};

/// @brief Creation status of the logger.
enum class CreationStatus : int {
    kError = -1,     // Error Creating Logger
    kSuccess = 0,    // Logger Created Successfully
    kExists = 1,     // Logger Already Exists
    kNotCreated = 2, // Logger Not Created
    kCount = 4       // Enum Counter
};

/// @brief Log pattern templates for different output styles.
enum class LogPattern : int {
    kSimple = 0,   // Minimal: [%n] %v
    kStandard = 1, // Default: [%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v
    kDetailed = 2, // Full: [%Y-%m-%d %H:%M:%S.%e] [%n] [%^%L%$] [thread %t] %v
    kThreadSafe =
        3 // With sequence number: [%H:%M:%S] [seq %i] [%n] [%^---%L---%$] %v
};

// ============================================================================
// Core Logger Creation and Management
// ============================================================================

/// @brief Create a logger with the specified name and file path.
/// Thread-safe for use in parallel execution contexts.
/// @param logger_name Unique identifier for this logger.
/// @param filepath File path where the logger will write logs.
/// @return CreationStatus indicating the result of logger creation.
/// @note If a logger with the same name already exists, kExists is returned.
CreationStatus CreateFileLogger(const std::string &logger_name,
                                const std::string &filepath);

// ============================================================================
// Parallel Execution Support: Shared File Sink
// ============================================================================

/// @brief Create a shared file sink that multiple loggers can write to.
/// This enables thread-safe concurrent logging from multiple models/threads
/// to the same output file. Use with CreateSharedLogger().
/// @param filepath Path to the shared log file.
/// @return CreationStatus indicating success or failure.
/// @note Thread-safe: Can be called from multiple threads simultaneously.
/// @note Creates the file sink once; subsequent calls return kExists.
CreationStatus CreateSharedFileSink(const std::string &filepath);

/// @brief Create a logger that writes to the shared file sink.
/// Multiple loggers can be created with different names but write to the
/// same file through the shared sink. Essential for parallel execution.
/// @param logger_name Unique identifier for this logger.
/// @return CreationStatus indicating the result of logger creation.
/// @note Thread-safe: Can be called concurrently from multiple threads.
/// @note Requires CreateSharedFileSink() to be called first with a file path.
/// @note If CreateSharedFileSink() wasn't called, creates a default sink to
/// "respond.log".
CreationStatus CreateSharedLogger(const std::string &logger_name);

/// @brief Sets the logging pattern template for all subsequent logger
/// creations.
/// @param pattern The LogPattern enum value to use.
/// @note Affects CreateFileLogger() and CreateSharedLogger() calls made after
/// this.
void SetLogPattern(LogPattern pattern);

/// @brief Gets the current logging pattern template.
/// @return The active LogPattern enum value.
LogPattern GetLogPattern();

/// @brief Sets the global flush interval for automatic buffer flushing.
/// @param seconds Interval in seconds for automatic flush (0 to disable
/// auto-flush).
/// @note Thread-safe configuration change.
void SetFlushInterval(int seconds);

/// @brief Flushes all active loggers, ensuring buffered output is written.
/// Useful when terminating parallel execution or before critical operations.
/// @note Thread-safe operation.
void FlushAllLoggers();

// ============================================================================
// Logging Functions
// ============================================================================

/// @brief Log a message as information level.
/// Thread-safe for concurrent calls from multiple threads.
/// @param logger_name Logger identifier (created via CreateFileLogger or
/// CreateSharedLogger).
/// @param message Message to log.
void LogInfo(const std::string &logger_name, const std::string &message);

/// @brief Log a message as warning level.
/// Thread-safe for concurrent calls from multiple threads.
/// @param logger_name Logger identifier.
/// @param message Message to log.
void LogWarning(const std::string &logger_name, const std::string &message);

/// @brief Log a message as error level.
/// Thread-safe for concurrent calls from multiple threads.
/// @param logger_name Logger identifier.
/// @param message Message to log.
void LogError(const std::string &logger_name, const std::string &message);

/// @brief Log a message as debug level.
/// Thread-safe for concurrent calls from multiple threads.
/// @param logger_name Logger identifier.
/// @param message Message to log.
void LogDebug(const std::string &logger_name, const std::string &message);

// ============================================================================
// Utility Functions
// ============================================================================

/// @brief Check if a logger with the given name exists.
/// @param logger_name Logger identifier to check.
/// @return CreationStatus::kExists if logger exists, kNotCreated otherwise.
/// @note Thread-safe query.
CreationStatus CheckLoggerExists(const std::string &logger_name);

/// @brief Retrieve detailed information about a logger.
/// @param logger_name Logger identifier to query.
/// @return String containing logger name, file path, level, and thread info.
/// @note Thread-safe operation.
std::string GetLoggerInfo(const std::string &logger_name);

/// @brief Set the logging level for a specific logger.
/// @param logger_name Logger identifier to configure.
/// @param level Log level: 0=trace, 1=debug, 2=info, 3=warn, 4=error,
/// 5=critical.
/// @return CreationStatus::kSuccess if level was set, kNotCreated if logger
/// doesn't exist.
void SetLoggerLevel(const std::string &logger_name, int level);

/// @brief Utility function to build a string message corresponding to the
/// exception.
/// @param error The exception object to extract the message from.
/// @param message The message object to append to the exception message.
/// @return A string containing the combined message and exception details.
std::string ConstructMessage(const std::exception &error, std::string message);
} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_LOGGING_HPP_

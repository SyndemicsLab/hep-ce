////////////////////////////////////////////////////////////////////////////////
// File: logging_test.cpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-22                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/utils/logging.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <thread>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

namespace hepce::utils::testing {

class LoggingTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear any existing loggers from previous tests
        spdlog::drop_all();

        // Create temporary log files for testing
        test_log_file_ = "/tmp/hepce_test.log";
        shared_log_file_ = "/tmp/hepce_shared.log";

        // Remove test files if they exist
        std::remove(test_log_file_.c_str());
        std::remove(shared_log_file_.c_str());
    }
    void TearDown() override {
        // Clean up loggers
        spdlog::drop_all();

        // Remove test files
        std::remove(test_log_file_.c_str());
        std::remove(shared_log_file_.c_str());
    }

    std::string test_log_file_;
    std::string shared_log_file_;

    // Helper to check if file contains a string
    bool FileContains(const std::string &filepath, const std::string &search) {
        std::ifstream file(filepath);
        if (!file.is_open())
            return false;

        std::string line;
        while (std::getline(file, line)) {
            if (line.find(search) != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    // Helper to count lines in file
    int CountLinesInFile(const std::string &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open())
            return 0;

        int count = 0;
        std::string line;
        while (std::getline(file, line)) {
            count++;
        }
        return count;
    }
};

// ============================================================================
// Test: CreateFileLogger Basic Functionality
// ============================================================================

TEST_F(LoggingTest, CreateFileLoggerSuccess) {
    CreationStatus status = CreateFileLogger("test_logger", test_log_file_);
    EXPECT_EQ(status, CreationStatus::kSuccess);

    // Verify logger exists
    auto logger = spdlog::get("test_logger");
    EXPECT_NE(logger, nullptr);
}

TEST_F(LoggingTest, CreateFileLoggerAlreadyExists) {
    CreateFileLogger("test_logger", test_log_file_);

    // Try creating again with same name
    CreationStatus status = CreateFileLogger("test_logger", test_log_file_);
    EXPECT_EQ(status, CreationStatus::kExists);
}

TEST_F(LoggingTest, CreateMultipleFileLoggers) {
    CreationStatus status1 = CreateFileLogger("logger1", test_log_file_);
    CreationStatus status2 = CreateFileLogger("logger2", test_log_file_);

    EXPECT_EQ(status1, CreationStatus::kSuccess);
    EXPECT_EQ(status2, CreationStatus::kSuccess);

    EXPECT_NE(spdlog::get("logger1"), nullptr);
    EXPECT_NE(spdlog::get("logger2"), nullptr);
}

// ============================================================================
// Test: Shared File Sink Functionality
// ============================================================================

TEST_F(LoggingTest, CreateSharedFileSink) {
    CreationStatus status = CreateSharedFileSink(shared_log_file_);
    EXPECT_EQ(status, CreationStatus::kSuccess);
}

TEST_F(LoggingTest, CreateSharedFileSinkCaching) {
    // Create sink first time
    CreationStatus status1 = CreateSharedFileSink(shared_log_file_);
    EXPECT_EQ(status1, CreationStatus::kSuccess);

    // Create sink again with same path (should reuse cached)
    CreationStatus status2 = CreateSharedFileSink(shared_log_file_);
    EXPECT_EQ(status2, CreationStatus::kSuccess);
}

TEST_F(LoggingTest, CreateSharedLogger) {
    CreateSharedFileSink(shared_log_file_);

    CreationStatus status = CreateSharedLogger("shared_logger");
    EXPECT_EQ(status, CreationStatus::kSuccess);

    auto logger = spdlog::get("shared_logger");
    EXPECT_NE(logger, nullptr);
}

TEST_F(LoggingTest, MultipleSharedLoggersToSameSink) {
    CreateSharedFileSink(shared_log_file_);

    CreationStatus status1 = CreateSharedLogger("shared_logger_1");
    CreationStatus status2 = CreateSharedLogger("shared_logger_2");

    EXPECT_EQ(status1, CreationStatus::kSuccess);
    EXPECT_EQ(status2, CreationStatus::kSuccess);

    // Both loggers should exist
    EXPECT_NE(spdlog::get("shared_logger_1"), nullptr);
    EXPECT_NE(spdlog::get("shared_logger_2"), nullptr);
}

// ============================================================================
// Test: Log Pattern Configuration
// ============================================================================

TEST_F(LoggingTest, SetAndGetLogPattern) {
    SetLogPattern(LogPattern::kDetailed);
    LogPattern pattern = GetLogPattern();
    EXPECT_EQ(pattern, LogPattern::kDetailed);
}

TEST_F(LoggingTest, LogPatternAffectsNewLoggers) {
    SetLogPattern(LogPattern::kSimple);
    CreateFileLogger("pattern_logger", test_log_file_);

    LogInfo("pattern_logger", "Test message");
    FlushAllLoggers();

    // File should exist and contain the message
    EXPECT_TRUE(FileContains(test_log_file_, "Test message"));
}

TEST_F(LoggingTest, ChangeLogPatternMultipleTimes) {
    SetLogPattern(LogPattern::kSimple);
    EXPECT_EQ(GetLogPattern(), LogPattern::kSimple);

    SetLogPattern(LogPattern::kStandard);
    EXPECT_EQ(GetLogPattern(), LogPattern::kStandard);

    SetLogPattern(LogPattern::kDetailed);
    EXPECT_EQ(GetLogPattern(), LogPattern::kDetailed);

    SetLogPattern(LogPattern::kThreadSafe);
    EXPECT_EQ(GetLogPattern(), LogPattern::kThreadSafe);
}

// ============================================================================
// Test: Flush Interval Configuration
// ============================================================================

TEST_F(LoggingTest, SetFlushInterval) {
    // Should not throw
    SetFlushInterval(0);
    SetFlushInterval(1);
    SetFlushInterval(5);
}

TEST_F(LoggingTest, SetFlushIntervalZeroForImmediateFlush) {
    SetFlushInterval(0);
    CreateFileLogger("flush_logger", test_log_file_);

    LogInfo("flush_logger", "Immediate flush");

    // Should be written immediately
    EXPECT_TRUE(FileContains(test_log_file_, "Immediate flush"));
}

// ============================================================================
// Test: Logging Functions
// ============================================================================

TEST_F(LoggingTest, LogInfo) {
    CreateFileLogger("test_logger", test_log_file_);

    LogInfo("test_logger", "Info message");
    FlushAllLoggers();

    EXPECT_TRUE(FileContains(test_log_file_, "Info message"));
}

TEST_F(LoggingTest, LogWarning) {
    CreateFileLogger("test_logger", test_log_file_);

    LogWarning("test_logger", "Warning message");
    FlushAllLoggers();

    EXPECT_TRUE(FileContains(test_log_file_, "Warning message"));
}

TEST_F(LoggingTest, LogError) {
    CreateFileLogger("test_logger", test_log_file_);

    LogError("test_logger", "Error message");
    FlushAllLoggers();

    EXPECT_TRUE(FileContains(test_log_file_, "Error message"));
}

TEST_F(LoggingTest, LogDebug) {
    CreateFileLogger("test_logger", test_log_file_);
    SetFlushInterval(0); // Immediate flush

    LogDebug("test_logger", "Debug message");
    FlushAllLoggers();

    // Verify logger exists and can log
    EXPECT_EQ(CheckLoggerExists("test_logger"), CreationStatus::kExists);
}

TEST_F(LoggingTest, MultipleLogMessages) {
    CreateFileLogger("test_logger", test_log_file_);
    SetFlushInterval(0); // Immediate flush

    // These should all complete without error
    LogInfo("test_logger", "Message 1");
    LogWarning("test_logger", "Message 2");
    LogError("test_logger", "Message 3");
    LogDebug("test_logger", "Message 4");
    FlushAllLoggers();

    // Verify logger exists
    EXPECT_EQ(CheckLoggerExists("test_logger"), CreationStatus::kExists);
}

// ============================================================================
// Test: Logger Utility Functions
// ============================================================================

TEST_F(LoggingTest, CheckLoggerExistsTrue) {
    CreateFileLogger("test_logger", test_log_file_);

    CreationStatus status = CheckLoggerExists("test_logger");
    EXPECT_EQ(status, CreationStatus::kExists);
}

TEST_F(LoggingTest, CheckLoggerExistsFalse) {
    CreationStatus status = CheckLoggerExists("nonexistent_logger");
    EXPECT_EQ(status, CreationStatus::kNotCreated);
}

TEST_F(LoggingTest, GetLoggerInfo) {
    CreateFileLogger("test_logger", test_log_file_);

    std::string info = GetLoggerInfo("test_logger");
    EXPECT_NE(info, "");
    EXPECT_NE(info.find("test_logger"), std::string::npos);
}

TEST_F(LoggingTest, GetLoggerInfoNonexistent) {
    std::string info = GetLoggerInfo("nonexistent_logger");
    EXPECT_NE(info.find("not found"), std::string::npos);
}

TEST_F(LoggingTest, SetLoggerLevel) {
    CreateFileLogger("test_logger", test_log_file_);

    // Should not throw
    SetLoggerLevel("test_logger", 0); // trace
    SetLoggerLevel("test_logger", 1); // debug
    SetLoggerLevel("test_logger", 2); // info
    SetLoggerLevel("test_logger", 3); // warn
    SetLoggerLevel("test_logger", 4); // error
    SetLoggerLevel("test_logger", 5); // critical
}

// ============================================================================
// Test: Flush All Loggers
// ============================================================================

TEST_F(LoggingTest, FlushAllLoggers) {
    CreateFileLogger("logger1", test_log_file_);
    CreateFileLogger("logger2", test_log_file_);

    LogInfo("logger1", "Message 1");
    LogInfo("logger2", "Message 2");

    // Should not throw
    FlushAllLoggers();

    EXPECT_TRUE(FileContains(test_log_file_, "Message 1"));
    EXPECT_TRUE(FileContains(test_log_file_, "Message 2"));
}

TEST_F(LoggingTest, ParallelSharedLoggingToSameFile) {
    CreationStatus sink_status = CreateSharedFileSink(shared_log_file_);
    EXPECT_EQ(sink_status, CreationStatus::kSuccess);

    const int num_threads = 4;
    const int messages_per_thread = 5;

    auto thread_func = [this](int thread_id) {
        std::string logger_name = "shared_logger_" + std::to_string(thread_id);
        CreationStatus status = CreateSharedLogger(logger_name);
        // Status could be kSuccess or kExists if concurrent threads create same
        // logger
        EXPECT_NE(status, CreationStatus::kError);

        for (int i = 0; i < messages_per_thread; ++i) {
            std::string message = "Thread " + std::to_string(thread_id) +
                                  " Message " + std::to_string(i);
            LogInfo(logger_name, message);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_func, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    FlushAllLoggers();

    // Verify at least one shared logger exists
    EXPECT_EQ(CheckLoggerExists("shared_logger_0"), CreationStatus::kExists);
}

TEST_F(LoggingTest, ConcurrentCreateSharedLogger) {
    CreateSharedFileSink(shared_log_file_);

    const int num_threads = 10;

    auto thread_func = [this](int thread_id) {
        std::string logger_name =
            "concurrent_logger_" + std::to_string(thread_id);
        CreationStatus status = CreateSharedLogger(logger_name);
        EXPECT_EQ(status, CreationStatus::kSuccess);
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_func, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    // All loggers should be created
    for (int i = 0; i < num_threads; ++i) {
        std::string logger_name = "concurrent_logger_" + std::to_string(i);
        EXPECT_NE(spdlog::get(logger_name), nullptr);
    }
}

// ============================================================================
// Test: Integration - Full Workflow
// ============================================================================

TEST_F(LoggingTest, FullWorkflowFileLogger) {
    // Create logger
    CreationStatus create_status =
        CreateFileLogger("full_test", test_log_file_);
    EXPECT_EQ(create_status, CreationStatus::kSuccess);

    // Set pattern
    SetLogPattern(LogPattern::kDetailed);
    EXPECT_EQ(GetLogPattern(), LogPattern::kDetailed);

    // Log messages
    LogInfo("full_test", "Starting full workflow test");
    LogWarning("full_test", "This is a warning");
    LogError("full_test", "This is an error");
    LogDebug("full_test", "Debug information");

    // Check logger exists
    EXPECT_EQ(CheckLoggerExists("full_test"), CreationStatus::kExists);

    // Get info
    std::string info = GetLoggerInfo("full_test");
    EXPECT_NE(info, "");

    // Set level
    SetLoggerLevel("full_test", 2); // info level

    // Flush
    FlushAllLoggers();

    // Verify output file
    EXPECT_TRUE(FileContains(test_log_file_, "Starting full workflow test"));
    EXPECT_TRUE(FileContains(test_log_file_, "This is a warning"));
    EXPECT_TRUE(FileContains(test_log_file_, "This is an error"));
}

TEST_F(LoggingTest, FullWorkflowSharedLogger) {
    // Setup shared logging
    CreationStatus sink_status = CreateSharedFileSink(shared_log_file_);
    EXPECT_EQ(sink_status, CreationStatus::kSuccess);

    SetLogPattern(LogPattern::kThreadSafe);
    EXPECT_EQ(GetLogPattern(), LogPattern::kThreadSafe);

    // Create shared loggers
    CreationStatus status1 = CreateSharedLogger("shared_1");
    CreationStatus status2 = CreateSharedLogger("shared_2");

    EXPECT_EQ(status1, CreationStatus::kSuccess);
    EXPECT_EQ(status2, CreationStatus::kSuccess);

    // Log from each
    LogInfo("shared_1", "Message from logger 1");
    LogInfo("shared_2", "Message from logger 2");

    // Verify loggers exist
    EXPECT_EQ(CheckLoggerExists("shared_1"), CreationStatus::kExists);
    EXPECT_EQ(CheckLoggerExists("shared_2"), CreationStatus::kExists);

    FlushAllLoggers();
}

TEST_F(LoggingTest, MixedFileAndSharedLoggers) {
    // Create file logger
    CreationStatus file_status =
        CreateFileLogger("file_logger", test_log_file_);

    // Create shared logger
    CreationStatus sink_status = CreateSharedFileSink(shared_log_file_);
    CreationStatus logger_status = CreateSharedLogger("shared_logger");

    EXPECT_EQ(file_status, CreationStatus::kSuccess);
    EXPECT_EQ(sink_status, CreationStatus::kSuccess);
    EXPECT_EQ(logger_status, CreationStatus::kSuccess);

    // Log to both
    LogInfo("file_logger", "File logger message");
    LogInfo("shared_logger", "Shared logger message");

    // Verify both exist
    EXPECT_EQ(CheckLoggerExists("file_logger"), CreationStatus::kExists);
    EXPECT_EQ(CheckLoggerExists("shared_logger"), CreationStatus::kExists);

    FlushAllLoggers();
}
}
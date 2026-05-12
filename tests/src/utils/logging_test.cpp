////////////////////////////////////////////////////////////////////////////////
// File: test_logging.cpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2025-05-01                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/utils/logging.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <thread>

#include <gtest/gtest.h>

class LoggingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(LoggingTest, LogInfo) {
    const std::string LOG_NAME = "info";
    const std::string LOG_FILE = "info.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);
    hepce::utils::LogInfo(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogWarning) {
    const std::string LOG_NAME = "warn";
    const std::string LOG_FILE = "warn.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);
    hepce::utils::LogWarning(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogError) {
    const std::string LOG_NAME = "error";
    const std::string LOG_FILE = "error.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);
    hepce::utils::LogError(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogDebug) {
    const std::string LOG_NAME = "debug";
    const std::string LOG_FILE = "debug.log";
    std::string expected = "Testing";
    std::string line;

    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);
    hepce::utils::LogDebug(LOG_NAME, expected);

    std::ifstream f(LOG_FILE);
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, CreateFileLoggerReturnsExistsWhenLoggerAlreadyRegistered) {
    const std::string LOG_NAME = "exists_case";
    const std::string LOG_FILE = "exists_case.log";

    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);
    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kExists);

    hepce::utils::DropLogger(LOG_NAME);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, DropLoggerAllowsRecreateWithSameName) {
    const std::string LOG_NAME = "drop_recreate";
    const std::string LOG_FILE = "drop_recreate.log";

    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);
    hepce::utils::DropLogger(LOG_NAME);
    ASSERT_EQ(hepce::utils::CreateFileLogger(LOG_NAME, LOG_FILE),
              hepce::utils::CreationStatus::kSuccess);

    hepce::utils::DropLogger(LOG_NAME);
    std::filesystem::remove(LOG_FILE);
}

TEST_F(LoggingTest, LogInfoCreatesDefaultLogFileWhenLoggerMissing) {
    const std::string LOG_NAME = "implicit_logger_creation";
    const std::string DEFAULT_LOG_FILE = "log.txt";
    const std::string expected = "auto-created message";
    std::string line;

    std::filesystem::remove(DEFAULT_LOG_FILE);
    hepce::utils::DropLogger(LOG_NAME);

    hepce::utils::LogInfo(LOG_NAME, expected);

    std::ifstream f(DEFAULT_LOG_FILE);
    ASSERT_TRUE(f.is_open());
    std::getline(f, line);
    f.close();

    ASSERT_TRUE(line.find(expected) != std::string::npos);

    hepce::utils::DropLogger(LOG_NAME);
    std::filesystem::remove(DEFAULT_LOG_FILE);
}

TEST_F(LoggingTest, ConstructMessageConcatenatesPrefixAndExceptionText) {
    const std::runtime_error err("boom");

    const std::string msg =
        hepce::utils::ConstructMessage(err, "while loading inputs");

    ASSERT_EQ(msg, "while loading inputs: boom");
}

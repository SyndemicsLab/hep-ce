cmake_minimum_required(VERSION 3.24)
project(HEPCETest CXX)

find_package(GTest REQUIRED CONFIG)

find_package(spdlog REQUIRED)

add_executable(hepceTest
  PersonTest.cpp
  SimTest.cpp
  EventTest.cpp
  InputTest.cpp
)

target_include_directories(hepceTest INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  "mocks/"
)

target_link_libraries(hepceTest
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  person
  simulation
  data
  event
  inputs
  spdlog::spdlog
)

# gtest_discover_tests(test)

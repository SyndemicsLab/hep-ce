cmake_minimum_required(VERSION 3.24)
project(HEPCETest CXX)

find_package(GTest REQUIRED)

add_executable(hepceTest
  PersonTest.cpp
  SimTest.cpp
  EventTest.cpp
)

target_include_directories(hepceTest INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hepceTest
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  person
  simulation
  data
)

# gtest_discover_tests(test)

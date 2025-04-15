cmake_minimum_required(VERSION 3.27)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)

project(globalHEPCE)

# Add option for building tests
option(BUILD_TESTS "enable HEPCE tests" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

include(MakeDependenciesAvailable)
add_subdirectory(src)

# Only compile tests if desired
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()

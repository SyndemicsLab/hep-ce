cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

project (globalHEPCE)

add_subdirectory(src)

# Only compile tests if desired
option(BUILD_TESTS "enable HEPCE tests" OFF)
if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()


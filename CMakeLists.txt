cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

# When both standard and multithreaded (mt) boost libraries exist, use standard
set(Boost_USE_MULTITHREADED OFF)

# Only compile tests if desired
option(BUILD_TESTS "enable HEPCE tests" ON)

# Set some basic project attributes
project (hepce_simulation)

add_subdirectory(libs/DataManagement)
add_subdirectory(src)

if (NOT BUILD_TESTS STREQUAL OFF)
  add_subdirectory(tests)
endif()


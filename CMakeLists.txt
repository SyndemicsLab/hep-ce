cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (HEPCESIMULATION)

# When both standard and multithreaded (mt) boost libraries exist, use standard
set(Boost_USE_MULTITHREADED OFF)

add_subdirectory(src)

# Only compile tests if desired
option(BUILD_TESTS "enable HEPCE tests" ON)

if (BUILD_TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(test)
endif()


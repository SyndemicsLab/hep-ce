cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (hepce_simulation)

# When both standard and multithreaded (mt) boost libraries exist, use standard
set(Boost_USE_MULTITHREADED OFF)

add_subdirectory(libs)
add_subdirectory(src)

# Only compile tests if desired
option(BUILD_TESTS "enable HEPCE tests" OFF)

if (BUILD_TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(test)
endif()

target_compile_definitions(src PRIVATE CMAKE_EXPORT_COMPILE_COMMANDS=1)


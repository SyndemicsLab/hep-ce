cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

# Set some basic project attributes
project (
  HEPCESIMULATION_SRC
  VERSION 2.0.0
  DESCRIPTION "A Micro-Simulation for Hepatitis C Virus Transmission"
)

include(GNUInstallDirs)

add_executable(HEPCESIMULATION_SRC main.cpp)

target_link_libraries(HEPCESIMULATION_SRC)

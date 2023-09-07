cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(simulation
  Simulation.cpp
)

set(SIMULATION_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(event PUBLIC ${SIMULATION_INCLUDES})
target_link_libraries(simulation
  person
  event
)

cmake_minimum_required(VERSION 3.20)

project(simulation)

find_package(spdlog REQUIRED)

find_package(DataManagement REQUIRED)

add_library(simulation Simulation.cpp)

set(SIMULATION_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(event PUBLIC ${SIMULATION_INCLUDES} ${DataManagement_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC event)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement)

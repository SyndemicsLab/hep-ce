cmake_minimum_required(VERSION 3.20)

project(simulation)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_library(${PROJECT_NAME} 
    Simulation.cpp 
    run.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC data)
target_link_libraries(${PROJECT_NAME} PUBLIC event)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
cmake_minimum_required(VERSION 3.20)

find_package(spdlog REQUIRED)

project(
    hepce
    VERSION 2.0.0
    DESCRIPTION "A Microsimulation for Hepatitis C Treatment Cost Effectiveness"
    )

add_subdirectory(event)
add_subdirectory(stats)
add_subdirectory(person)
add_subdirectory(simulation)
add_subdirectory(data)
add_subdirectory(utils)
add_subdirectory(cost)

add_executable(hepce main.cpp)

target_include_directories(hepce PRIVATE
    "cost/include"
    "data/include"
    "event/include"
    "person/include"
    "simulation/include"
    "utils/include"
    "stats/include"
)

target_link_libraries(hepce
    stats    
    event
    person
    simulation
    utils
    data
    spdlog::spdlog
)

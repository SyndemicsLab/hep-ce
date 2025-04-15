cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_MakeAvailable(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

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
add_subdirectory(utility)

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

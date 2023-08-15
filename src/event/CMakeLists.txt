cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(event
    ScreenageLinking.cpp
    Screening.cpp
    VoluntaryRelinking.cpp
)

target_include_directories(event INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "include/"
)

target_link_libraries(event
    person
    libs
)
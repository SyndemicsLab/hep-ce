cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(data
    Data.cpp
)

target_include_directories(data INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "include/"
)

target_link_libraries(data
    person
    event
)
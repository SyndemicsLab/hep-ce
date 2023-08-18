cmake_minimum_required(VERSION 3.19)
set (CMAKE_CXX_STANDARD 20)

add_library(data
    Data.cpp
)

set(DATA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(data PUBLIC ${DATA_INCLUDES})

target_link_libraries(data
    person
    libs
    event
)
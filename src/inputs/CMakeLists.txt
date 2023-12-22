cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(inputs Inputs.cpp)

set(INPUTS_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(inputs PUBLIC ${INPUTS_INCLUDES})

target_link_libraries(inputs data)

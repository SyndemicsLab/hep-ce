cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(utils
    Utils.cpp
)
target_link_libraries(utils)

target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

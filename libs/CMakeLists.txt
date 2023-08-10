cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(libs
    sqlite3/sqlite3.c
)

target_include_directories(libs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3/
)
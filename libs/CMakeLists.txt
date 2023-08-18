cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(libs
    sqlite3/sqlite3.c
)

set(LIB_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/sqlite3")

target_include_directories(libs PUBLIC ${LIB_INCLUDES})
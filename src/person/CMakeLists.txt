cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(person Person.cpp)

set(PERSON_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(person PUBLIC ${PERSON_INCLUDES})
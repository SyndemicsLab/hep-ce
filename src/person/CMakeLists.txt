cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(person
    Person.cpp
)

target_include_directories(person INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "include/"
)

target_link_libraries(person
    
)
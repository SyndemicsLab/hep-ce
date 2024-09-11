cmake_minimum_required(VERSION 3.20)

project(stats)

add_library(${PROJECT_NAME} 
    Decider.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
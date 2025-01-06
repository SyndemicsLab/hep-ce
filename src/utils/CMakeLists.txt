cmake_minimum_required(VERSION 3.19)
set (CMAKE_CXX_STANDARD 20)

project(utils)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_sources(${PROJECT_NAME} 
    PRIVATE
        Utils.cpp
)

target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

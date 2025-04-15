cmake_minimum_required(VERSION 3.20)

project(data)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
        DataWriter.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        cost
        person
        spdlog::spdlog
        DataManagement::DataManagement
        utils
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

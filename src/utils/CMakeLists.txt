cmake_minimum_required(VERSION 3.20)
set (CMAKE_CXX_STANDARD 20)

project(utils)
find_package(spdlog)

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
        Utils.cpp
)

target_include_directories(utils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(utils
  PRIVATE
      spdlog::spdlog
      DataManagement::DataManagement
)

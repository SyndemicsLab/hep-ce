cmake_minimum_required(VERSION 3.20)
set (CMAKE_CXX_STANDARD 20)

project(hepce)

find_package(spdlog REQUIRED)

find_package(DataManagement REQUIRED
    HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_executable(hepce main.cpp run.cpp)

add_subdirectory(event)
add_subdirectory(person)
add_subdirectory(simulation)
add_subdirectory(utils)
add_subdirectory(cost)

include(CMakePrintHelpers)
cmake_print_variables(DataManagement_INCLUDE_DIR)

target_link_libraries(${PROJECT_NAME} PUBLIC event)
target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC simulation)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE DataManagement::DataManagement)

# target_include_directories(${PROJECT_NAME} PUBLIC ${DataManagement_INCLUDE_DIR})
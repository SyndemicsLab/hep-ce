cmake_minimum_required(VERSION 3.20)

project(event)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

find_package(OpenMP REQUIRED)

add_library(event
    Aging.cpp
    BehaviorChanges.cpp
    Clearance.cpp
    Death.cpp
    FibrosisProgression.cpp
    FibrosisStaging.cpp
    Infections.cpp
    Linking.cpp
    MOUD.cpp
    Overdose.cpp
    Screening.cpp
    Treatment.cpp
    VoluntaryRelinking.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC cost)
target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC stats)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)

target_include_directories(event PUBLIC "include/")

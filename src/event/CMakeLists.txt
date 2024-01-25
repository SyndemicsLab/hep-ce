cmake_minimum_required(VERSION 3.19)

project(event)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_library(event
    Aging.cpp
    BehaviorChanges.cpp
    Clearance.cpp
    Death.cpp
    DiseaseProgression.cpp
    FibrosisStaging.cpp
    Infections.cpp
    Overdose.cpp
    Linking.cpp
    Screening.cpp
    Treatment.cpp
    VoluntaryRelinking.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)
target_link_libraries(${PROJECT_NAME} PUBLIC cost)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)

set(EVENT_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(event PUBLIC ${EVENT_INCLUDES})

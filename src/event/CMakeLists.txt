cmake_minimum_required(VERSION 3.20)

project(event)

find_package(spdlog REQUIRED)

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
        Aging.cpp
        BehaviorChanges.cpp
        Clearance.cpp
        Death.cpp
        Event.cpp
        EventFactory.cpp
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

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        cost
        person
        stats
        spdlog::spdlog
        DataManagement::DataManagement
        utils
)

target_include_directories(event PUBLIC "include/")

cmake_minimum_required(VERSION 3.20)

project(event)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_MakeAvailable(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

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
        HCVInfections.cpp
        HCVLinking.cpp
        HCVScreening.cpp
        HCVTreatment.cpp
        HIVInfections.cpp
        HIVLinking.cpp
        HIVScreening.cpp
        Linking.cpp
        MOUD.cpp
        Overdose.cpp
        VoluntaryRelinking.cpp
        Pregnancy.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        cost
        utility
        person
        stats
        spdlog::spdlog
        DataManagement::DataManagement
        utils
)

target_include_directories(event PUBLIC "include/")

cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 20)

add_library(event
    Aging.cpp
    BehaviorChanges.cpp
    Clearance.cpp
    Death.cpp
    DiseaseProgression.cpp
    Fibrosis.cpp
    Infections.cpp
    Overdose.cpp
    ScreenageLinking.cpp
    Screening.cpp
    Treatment.cpp
    VoluntaryRelinking.cpp
)

set(EVENT_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(event PUBLIC ${EVENT_INCLUDES})

target_link_libraries(event
    person
    data
    utils
)

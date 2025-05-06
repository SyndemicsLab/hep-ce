cmake_minimum_required(VERSION 3.27)
project(hepce_tests VERSION ${HEPCE_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT HEPCE_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but HEPCE_BUILD_TESTS is OFF!")
    return()
endif()


enable_testing()

add_executable(${PROJECT_NAME}
    src/event/base/aging_test.cpp
    src/event/base/death_test.cpp
    src/event/behavior/behavior_changes_test.cpp
    src/event/behavior/pregnancy_test.cpp
    src/event/behavior/overdose_test.cpp
    src/event/behavior/moud_test.cpp
    src/event/fibrosis/progression_test.cpp
    src/event/fibrosis/staging_test.cpp
    src/event/hcv/clearance_test.cpp
    src/event/hcv/infection_test.cpp
    src/event/hcv/linking_test.cpp
    src/event/hcv/screening_test.cpp
    src/event/hcv/treatment_test.cpp
    src/event/hcv/voluntary_relink_test.cpp
    src/event/hiv/infection_test.cpp
    src/event/hiv/linking_test.cpp
    src/event/hiv/screening_test.cpp
    src/data/writer_test.cpp
    src/utils/logging_test.cpp
    src/model/person_test.cpp
#   src/model/simulation_test.cpp
#   src/model/cost_test.cpp
)

# BE VERY CAREFUL WITH GTEST AS ITS A PAIN IN THE BUTT
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        hepce
        GTest::gmock_main
        datamanagement::datamanagement
        spdlog::spdlog
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "mocks/"
        "constants/"
)

gtest_discover_tests(${PROJECT_NAME})

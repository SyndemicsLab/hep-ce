if(HEPCE_BUILD_PYBINDINGS OR HEPCE_BUILD_ALL)
    message(STATUS "Generating Python Bindings")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    add_subdirectory(extras/hepcepy)
endif()

if(HEPCE_BUILD_EXECUTABLE OR HEPCE_BUILD_ALL)
    message(STATUS "Building Executable")
    add_subdirectory(extras/executable)
endif()

if(HEPCE_BUILD_TESTS OR HEPCE_BUILD_ALL)
    message(STATUS "Generating tests")
    if(HEPCE_CALCULATE_COVERAGE)
        message(STATUS "Calculating Code Coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    endif()
    enable_testing()
    add_subdirectory(tests)
endif()

if(HEPCE_BUILD_BENCH OR HEPCE_BUILD_ALL)
    message(STATUS "Generating benchmarks")
    add_subdirectory(extras/benchmarking)
endif()

if(HEPCE_RUN_OMP)
    message(STATUS "Running with OMP")
endif()
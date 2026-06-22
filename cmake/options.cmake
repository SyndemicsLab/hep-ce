option(HEPCE_BUILD_ALL "Build all artifacts" OFF)

# build position independent code
option(HEPCE_BUILD_PIC "Build position independent code (-fPIC)" OFF)

# shared_libs options
option(BUILD_SHARED_LIBS "Enable shared libraries" ON)

# build documentation
option(HEPCE_BUILD_DOCS "Build Documentation" ON)

option(HEPCE_BUILD_EXECUTABLE "Build Executable for HEPCE" ON)

# testing options
option(HEPCE_BUILD_TESTS "Build tests" OFF)

# coverage options (only valid if HEPCE_BUILD_TESTS is ON)
option(HEPCE_CALCULATE_COVERAGE "Calculate Code Coverage" OFF)

# bench options
option(HEPCE_BUILD_BENCH "Build benchmarks" OFF)

# warning options
option(HEPCE_BUILD_WARNINGS "Enable compiler warnings" OFF)
option(HEPCE_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any simdemics exceptions" OFF)

# stop on warnings
option(HEPCE_STOP_ON_WARNINGS "Stop Execution if a warning is occurred" OFF)

# run with omp
option(HEPCE_RUN_OMP "Enable omp runtime" OFF)

# install options
option(HEPCE_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(HEPCE_INSTALL "Generate the install target" ${HEPCE_MASTER_PROJECT})


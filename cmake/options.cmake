option(HEPCE_BUILD_ALL "Build all artifacts" OFF)

# build position independent code
option(HEPCE_BUILD_PIC "Build position independent code (-fPIC)" OFF)

option(HEPCE_BUILD_EXECUTABLE "Build Executable for HEPCE" ON)

# testing options
option(HEPCE_BUILD_TESTS "Build tests" ON)

# coverage options (only valid if HEPCE_BUILD_TESTS is ON)
option(HEPCE_CALCULATE_COVERAGE "Calculate Code Coverage" OFF)

# pybind options
option(HEPCE_BUILD_PYBINDINGS "Build python bindings" OFF)

# bench options
option(HEPCE_BUILD_BENCH "Build benchmarks (Requires https://github.com/google/benchmark.git to be installed)" OFF)

# shared_libs options
option(HEPCE_BUILD_SHARED_LIBS "Enable shared libraries" ${BUILD_SHARED_LIBS})

# warning options
option(HEPCE_BUILD_WARNINGS "Enable compiler warnings" OFF)

# install options
option(HEPCE_SYSTEM_INCLUDES "Include as system headers (skip for clang-tidy)." OFF)
option(HEPCE_INSTALL "Generate the install target" ${HEPCE_MASTER_PROJECT})
option(HEPCE_NO_EXCEPTIONS "Compile with -fno-exceptions. Call abort() on any simdemics exceptions" OFF)

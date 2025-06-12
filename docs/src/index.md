# Hepatitis-C Cost Effectiveness

<a href="https://www.syndemicslab.org/hep-ce"><img align="right" src="HEPCE_logo.png" alt="HEPCE Logo" height="120" /></a>

This repository houses a complete rewrite of the original HEPCE model, first created by the [Syndemics Lab](https://www.syndemicslab.org) over the past two decades. It has been developed with a focus on 3 primary goals:

1. Improve the maintainability of the model
2. Allow for easy design of new events
3. Implement the model as a true discrete-event simulation.

## HEPCEv1

The [original HEPCE model](https://github.com/SyndemicsLab/hep-ce-v1) was built with the C++ programming languages.

## CMake

HEPCE makes full use of the CMake build system. It is a common tool used throughout the C++ user-base and we utilize it for dependency management, linking, and testing. As C++ has poor package management, we intentionally decided to move our focus away from tools such as conan and vcpkg and stay with pure CMake. Not to say we would never publish with such package managers, but it is not a core focus of the refactor/engineering team.

We natively support 4 different build workflows with the `CMakePresets.json` file. They are:

1. `gcc-release`
2. `gcc-debug`
3. `gcc-release-cluster`
4. `gcc-debug-cluster`

Unless you are explicitly using a linux based computing cluster, we highly recommend choosing one of the first two build processes. In the future, we do intend to expand to additional compilers and operating systems beyond GCC and Linux.

Overall, we make use of 11 custom CMake variables. They are found in the [options.cmake file](https://github.com/SyndemicsLab/hep-ce/blob/main/cmake/options.cmake) and all are set accordingly in the `CMakePresets.json`.

## Dependencies

We make abundant use of the CMake `FetchContent` feature released in CMake 3.11. We utilize features added in CMake 3.24 to check if the package is previously installed, so the minimum required version of CMake is **3.24**.

The required dependencies are:

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [spdlog](https://github.com/gabime/spdlog)

For tests we require:

- [GoogleTest](https://github.com/google/googletest)

## Build

This is, by nature, a C++ library. This means that the default behavior is not to provide an executable for the model, but rather a set of callable functions to design your own model. However, we have a distinct use case for an executable and building a model, and as such we provide the ability to build and install this executable. This is directly controlled via the `HEPCE_BUILD_EXECUTABLE` variable.

### Local

If you would like to clone and build this locally, it is a relatively straightforward process:

```shell
git clone https://github.com/SyndemicsLab/hep-ce.git
cd hep-ce
cmake --workflow --preset gcc-release
```

And then the model is build and installed. Our default location is a build directory in the repository, but the CMake Install Directory can be pointed to wherever the user desires.

### Fetch Content

If you would like to make use of Fetch Content to extract the library:

```cmake
include(FetchContent)
FetchContent_Declare(
    hepce
    GIT_REPOSITORY https://github.com/SyndemicsLab/hep-ce.git
    GIT_TAG main
)
option(HEPCE_INSTALL "Enable install for respond project" ON)
option(HEPCE_BUILD_TESTS "Disable testing for HEPCE" OFF)
FetchContent_MakeAvailable(respond)
```

### Script

If you would prefer a single, all in one, script. Our team has developed a script that works on any linux environment and finds packages wherever available. As such, the user needs simply run:

```shell
./tools/build.sh
```

## Running the HEPCE Executable

If you choose to use our provided executable, running the model requires a config file and a database. The database can be created via the [example schema](extras/examples/inputs.db.sql). Each distinct run is put into a folder titled `input<number>` where the number is replaced with the ID of the input. Then, after these folders are created and HEPCE is built we simply run the command:

```bash
./build/extras/executable/hepce_exe <input_start> <input_end>
```

Next: [Installation](installation.md)

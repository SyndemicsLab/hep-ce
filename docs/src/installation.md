# Building & Installation

There are several pre-configured builds provided for the `HEP-CE` simulation model.
We recommend either building the source code directly, if you intend to use the
model standalone, or utilizing CMake's [FetchContent][fetchcontent] feature to include the model as a dependency. We have plans to provide packages to package managers for Linux distributions directly in the future.

## Dependencies

Assuming you have the required building tools (i.e. [GNU Compiler Collection (GCC)][gcc], [CMake][cmake], [Ninja][ninja]), you still need:

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [spdlog](https://github.com/gabime/spdlog)

**Note:** DataManagement is a Syndemics Lab library and under heavy development. It is *NOT* recommended to attempt to install a separate version.

If you build from source, missing dependencies are installed via CMake's [FetchContent][fetchcontent].
You will need to install these packages for yourself if you intend to use a specific version or release.

## Building From Source
In order to build `HEP-CE` from source, clone the repository and use CMake. We
recommend building the `gcc-release` preset, included in the following
example:

```bash
git clone https://github.com/SyndemicsLab/hep-ce.git
cd hep-ce
cmake --workflow --preset gcc-release
```

### Presets

Definitions for presets are located in `CMakePresets.json`, and we encourage their use, especially if you intend to contribute to or to use the library downstream.

There are currently five presets provided:

| Preset Name | Description |
| ----------- | ----------- |
| `gcc-release` | Builds the simulation model, including the executable, with CPU parallelization via OpenMP |
| `gcc-debug` | Builds the simulation model, including the executable and tests, *without* parallelization and runs unit tests |
| `gcc-release-cluster` | `gcc-release` with additional settings for use with the [Boston University Shared Computing Cluster (SCC)][scc] |
| `gcc-debug-cluster` | `gcc-release` with additional settings for use with the [SCC][scc] |
| `gcc-release-strict` | `gcc-release-cluster` with additional settings that cause the model to treat warnings as errors |

We also provide a debug version that is slower, but prints more to the logger to help debug problems:

```bash
cmake --workflow gcc-debug
```

## Installing with CMake

Since CMake 3.11, the `FetchContent` tool has been available to users. Early on in the design process, we made the decision to progress under the assumption that CMake will handle our dependencies and additional package managers can be installed as necessary and included in toolchain files. To install via `FetchContent` simply do:

```cmake
include(FetchContent)
FetchContent_Declare(
    hepce
    GIT_REPOSITORY https://github.com/SyndemicsLab/hep-ce.git
    GIT_TAG main
)
option(HEPCE_INSTALL "Enable install for hep-ce project" ON)
option(HEPCE_BUILD_TESTS "Disable testing for hep-ce" OFF)
FetchContent_MakeAvailable(hepce)
```

This should produce a corresponding `hepceConfig.cmake` file for CMake linking and installation.

Previous: [Home](index.md)

Next: [Data](data.md)

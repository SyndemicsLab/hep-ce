# Building & Installation

`HEP-CE` uses the [CMake][cmake] build system. We use many of the features of
CMake to avoid needing additional environment management dependencies, and as
such require CMake 3.24 or newer to build.

There are several pre-configured builds provided for the `HEP-CE` simulation
model, and these presets should cover the majority of use cases.
We recommend either building the source code directly, if you intend to use the
model standalone, or utilizing CMake's [FetchContent][fetchcontent] feature to
include the model as a dependency in your project. We have plans to provide
packages to package managers for Linux distributions, directly, in the future.
If you want or need to see all build options, these can be found in the
[`options.cmake`][hepceoptions] file.

## Dependencies

Assuming you have the required building tools (i.e. [GNU Compiler Collection (GCC)][gcc], [CMake][cmake], [Ninja][ninja]), you still need:

- [DataManagement](https://github.com/SyndemicsLab/DataManagement)
- [spdlog](https://github.com/gabime/spdlog)
- [GoogleTest][gtest] (optional, only used when building tests)

**Note:** DataManagement is a Syndemics Lab library and under heavy development. It is *NOT* recommended to attempt to install a separate version.

If you build from source, missing dependencies are installed via CMake's
[FetchContent][fetchcontent]. You will need to install these packages for
yourself if you intend to use a specific version or release.

## Building From Source
In order to build `HEP-CE` from source, clone the repository and use
[CMake][cmake]. We recommend building the `gcc-release` preset, included in the
following example:

```bash
git clone https://github.com/SyndemicsLab/hep-ce.git
cd hep-ce
cmake --workflow --preset gcc-release
```

### Presets

Definitions for presets are located in `CMakePresets.json`, and we encourage
their use, especially if you intend to contribute to or to use the library
downstream. Currently, presets include:

| Preset Name | Description |
| ----------- | ----------- |
| `gcc-release` | Builds the simulation model, including the executable, with CPU parallelization via OpenMP |
| `gcc-debug` | Builds the simulation model, including the executable and tests, *without* parallelization and runs unit tests |
| `gcc-release-cluster` | `gcc-release` with additional settings for use with the [Boston University Shared Computing Cluster (SCC)][scc] |
| `gcc-debug-cluster` | `gcc-release` with additional settings for use with the [SCC][scc] |
| `gcc-release-strict` | `gcc-release` with additional settings that cause the model to treat warnings as errors |
| `gcc-release-strict-cluster` | `gcc-release-cluster` with additional settings that cause the model to treat warnings as errors |

To build any of these presets instead, simply replace `gcc-release` in the
example with the desired preset name.

### Using HEP-CE in Your Project

Using CMake's [FetchContent][fetchcontent] feature, you can incorporate the
model API in your project by including the following in your `CMakeLists.txt`:

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

This produces a `hepceConfig.cmake` file that CMake uses for linking and
installation.

### Building on UNIX

For convenience, we also provide a Bash script that builds the default release
on UNIX-based systems. To use this, run

```bash
scripts/build.sh
```

To build alternate presets, use the command `scripts/build.sh -h` to see the
options in the help dialog.



<div class="section_buttons">

| Previous     |                  Next |
|:-------------|----------------------:|
| [Home][home] | [Inputs & Outputs][data] |

</div>

[cmake]: https://cmake.org
[data]: data.md
[fetchcontent]: https://cmake.org/cmake/help/latest/module/FetchContent.html
[gcc]: https://gcc.gnu.org/
[gtest]: https://github.com/google/googletest
[hepceoptions]: https://github.com/SyndemicsLab/hep-ce/blob/main/cmake/options.cmake
[home]: index.md
[ninja]: https://ninja-build.org
[scc]: https://www.bu.edu/tech/support/research/computing-resources/scc/

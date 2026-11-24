# Packaging Strategy

HEP-CE will support two complementary packaging paths:

- Native Linux packages for Debian-family and RPM-family distributions
- A portable TGZ package for environments where native dependency packages are
  unavailable

The native packages are the first priority. The TGZ package provides a
cross-platform fallback and is intended to contain the dependencies needed for
both runtime use and downstream CMake consumption.

## Dependency Resolution

Dependency resolution happens while configuring and packaging HEP-CE, not from
the installed CMake package configuration file:

1. Try to find an installed dependency with CMake.
2. If it is unavailable during the HEP-CE build, fetch and build it when the
   dependency supports that workflow.
3. For native packages, declare the dependency for the operating system package
   manager.
4. For the TGZ package, install the dependency artifacts and CMake metadata into
   the package.

`find_dependency()` remains in `hepceConfig.cmake` so downstream CMake projects
can import dependency targets. It verifies that the dependency targets are
available; it does not install packages or invoke an operating system package
manager.

The `HEPCE_BUNDLE_DEPENDENCIES` CMake option selects the portable fallback. Its
default is `OFF`, which prefers installed dependencies and produces native
packages with external dependency declarations. Setting it to `ON` forces the
fetched dependency sources to be used and includes their install components in
the portable package.

For example:

```bash
cmake --preset package-shared -DHEPCE_BUNDLE_DEPENDENCIES=ON
cmake --build build/shared
cpack --config build/shared/CPackConfig.cmake -G TGZ
```

## Native Linux Packages

The DEB package will declare dependencies through CPack's Debian package
metadata. The RPM package will declare dependencies through CPack's RPM package
metadata. When users install these packages with `apt`, `dnf`, or a compatible
package manager, the package manager can resolve and install the declared
dependencies.

The dependency names may differ between Debian-family and RPM-family
distributions, so each package format needs its own dependency declarations.
Both DEB and RPM packages are part of the initial Linux packaging scope.

## Windows Installer

Windows packaging uses CPack's NSIS generator. The `nsis-shared` and
`nsis-static` package presets configure installation with
`HEPCE_BUNDLE_DEPENDENCIES=ON`, so fetched dependency install artifacts are
included in the installer. The executable and library are separate installer
components, and selecting the executable also selects the library.

For example, on a Windows machine with CMake, Ninja, a C++ compiler, Git, and
NSIS installed:

```bash
cmake --preset windows-package-shared
cmake --build build/windows-shared --config Release
cpack --preset nsis-shared
```

The generated installer is written to the Windows build directory. Windows
packaging is not validated on the current Linux development machine.

## Portable TGZ Package

The TGZ package cannot rely on the host distribution to install dependencies.
Its install rules must therefore include the dependency artifacts needed by
HEP-CE. Where practical, dependencies should be statically linked into HEP-CE
to reduce the number of runtime files that need to be bundled.

The TGZ must also include the CMake package metadata required by downstream
projects using:

```cmake
find_package(hepce REQUIRED)
target_link_libraries(app PRIVATE hepce::hepce_model)
```

Bundling only shared or static library files is not sufficient for this use
case. The package must also provide usable imported targets and dependency
configuration files, or otherwise make those targets available to the installed
HEP-CE targets.

Portable mode bundles the fetched SQLiteCpp and spdlog install artifacts,
including their headers and CMake package metadata. Boost headers, OpenMP,
Threads, and the platform C/C++ runtime remain host requirements; the TGZ is
portable across compatible systems, not a universal binary.

## Scope and Limitations

The bundled dependency scope includes HEP-CE's direct link dependencies,
including SQLiteCpp and spdlog, together with the artifacts required by the
selected platform and linkage model. `Boost::headers` is a header dependency,
and OpenMP may also require compiler- or platform-provided runtime components.

System components such as the C library and compiler runtime are not expected to
be bundled universally. Their treatment must remain platform-specific.

RPM package validation may require a Fedora- or RHEL-compatible environment.
An isolated container can provide that environment, but its runtime must be
available before validation begins.

## Sources

- [CMake `find_dependency()`](https://cmake.org/cmake/help/latest/module/CMakeFindDependencyMacro.html)
- [CMake Importing and Exporting Guide](https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html)
- [CMake Creating Relocatable Packages](https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html#creating-relocatable-packages)
- [CMake `FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html)
- [CPack DEB Generator](https://cmake.org/cmake/help/latest/cpack_gen/deb.html)
- [CPack RPM Generator](https://cmake.org/cmake/help/latest/cpack_gen/rpm.html)
- [CPack Archive Generator](https://cmake.org/cmake/help/latest/cpack_gen/archive.html)

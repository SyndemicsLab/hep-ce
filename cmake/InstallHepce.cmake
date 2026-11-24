message(STATUS "Installing hepce version ${HEPCE_VERSION} to ${CMAKE_INSTALL_FULL_LIBDIR} and ${CMAKE_INSTALL_FULL_INCLUDEDIR}...")

# This defines where things get installed. The default is to install to the
# system level directories (e.g. /usr/lib, /usr/hepce/include). We will want
# to change this to allow for user level install too via the
# CMAKE_INSTALL_PREFIX variable.
install(TARGETS hepce_model
    EXPORT hepceTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT hepce_library
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT hepce_library
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT hepce_library
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT hepce_library
)

if(TARGET hepce_exe)
    install(TARGETS hepce_exe
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT hepce_executable
    )
endif()

include(CMakePackageConfigHelpers)

# These config files are all initially created in the cmake build and then written to the default install location (e.g. `/usr/lib`) with the extra path `cmake/hepce/`...

# Setup the version config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/hepce/hepceConfigVersion.cmake"
    VERSION ${HEPCE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# export the targets to a file and add the prefix. This file is used by the
# config file to find the compiled targets during `find_package(hepce)` calls.
export(EXPORT hepceTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/hepce/hepceTargets.cmake"
  NAMESPACE hepce::
)

# Creates the config file necessary for CMake to use `find_package(hepce)` 
# after being installed.
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/hepceConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/hepce/hepceConfig.cmake"
    @ONLY
)

# Define where to install the config files and install the targets before the 
# config files. This is so that the config files can actually find the targets
set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/hepce)

# Exports the cmake linkable targets, associates it with the cmake file,
# provides the namespace, and passes it to the config files location
install(
    EXPORT hepceTargets
    FILE hepceTargets.cmake
    NAMESPACE hepce::
    DESTINATION ${ConfigPackageLocation}
    COMPONENT hepce_library
)

# Finally, install the config files
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/hepce/hepceConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/hepce/hepceConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/hepce"
    COMPONENT hepce_library
)

#-------------------------------------------------------------------------------
# CPack: This helps us create various installers. All options are controlled in
# the `hepceCPackOptions.cmake.in` config file. Please do not change CPack
# itself, only the Options file!
#-------------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/hepceCPackOptions.cmake.in"
    "${PROJECT_BINARY_DIR}/hepceCPackOptions.cmake"
    @ONLY
)
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/hepceCPackOptions.cmake")

include(CPack)

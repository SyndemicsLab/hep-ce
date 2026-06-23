message(STATUS "Installing hepce version ${HEPCE_VERSION}...")

set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/hepce")
set(export_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/hepce")
set(version_config_file "${CMAKE_CURRENT_BINARY_DIR}/hepceConfigVersion.cmake")

# ------------------------------------------------------------------------------
# Include files
# ------------------------------------------------------------------------------
message("Installing hep-ce to ${CMAKE_INSTALL_FULL_LIBDIR} and ${CMAKE_INSTALL_FULL_INCLUDEDIR}...")

# install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(
    TARGETS hepce_model
    EXPORT hepceTargets
    FILE_SET HEADERS
)

install(
    EXPORT hepceTargets
    DESTINATION ${export_cmake_dir}
    NAMESPACE hepce::
)

include(CMakePackageConfigHelpers)

configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/hepceConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/hepceConfig.cmake"
    @ONLY
)
set(project_config "${PROJECT_BINARY_DIR}/hepceConfig.cmake")

write_basic_package_version_file(
  ${version_config_file}
  COMPATIBILITY ExactVersion
)

install(
    FILES
        ${project_config}
        ${version_config_file}
    DESTINATION ${export_cmake_dir}
)

#-------------------------------------------------------------------------------
# CPack Options for HEP-CE
#-------------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/hepceCPackOptions.cmake.in"
    "${PROJECT_BINARY_DIR}/hepceCPackOptions.cmake"
    @ONLY
)
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/hepceCPackOptions.cmake")

#-------------------------------------------------------------------------------
# Support creation of installable packages
#-------------------------------------------------------------------------------
include(CPack)

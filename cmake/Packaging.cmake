# Include GNUInstallDirs for installation directory variables
include(GNUInstallDirs)

# Define installation directory for the package configuration files
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${TURINGFORGE_PACKAGE_NAME})

# Install the library and headers
install(TARGETS turingforge
        EXPORT Turingforge-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Ensure the exported target has the name Turingforge and not turingforge
set_target_properties(turingforge PROPERTIES EXPORT_NAME Turingforge)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export the targets to a script
install(EXPORT Turingforge-targets
        FILE TuringforgeTargets.cmake
        NAMESPACE Turingforge::
        DESTINATION ${INSTALL_CONFIGDIR}
)

# Package Configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/../cmake/TuringforgeConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT Turingforge-targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeTargets.cmake
        NAMESPACE Turingforge::)

# Register package in the User Package Registry
export(PACKAGE Turingforge)
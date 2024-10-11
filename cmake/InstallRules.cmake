if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_INSTALL_INCLUDEDIR include/turingforge CACHE PATH "")
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package turingforge)

install(
        DIRECTORY
        include/
        "${PROJECT_BINARY_DIR}/export/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT turingforge_Development
)

install(
        TARGETS turingforge_turingforge
        EXPORT turingforgeTargets
        RUNTIME #
        COMPONENT turingforge_Runtime
        LIBRARY #
        COMPONENT turingforge_Runtime
        NAMELINK_COMPONENT turingforge_Development
        ARCHIVE #
        COMPONENT turingforge_Development
        INCLUDES #
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
        "${package}ConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
        turingforge_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/${package}"
        CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(turingforge_INSTALL_CMAKEDIR)

install(
        FILES cmake/install-config.cmake
        DESTINATION "${turingforge_INSTALL_CMAKEDIR}"
        RENAME "${package}Config.cmake"
        COMPONENT turingforge_Development
)

install(
        FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
        DESTINATION "${turingforge_INSTALL_CMAKEDIR}"
        COMPONENT turingforge_Development
)

install(
        EXPORT turingforgeTargets
        NAMESPACE turingforge::
        DESTINATION "${turingforge_INSTALL_CMAKEDIR}"
        COMPONENT turingforge_Development
)

if(PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif()
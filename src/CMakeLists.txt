##############################################
# Find/Install system dependencies
include(FetchContent)
include(ExternalProject)

# TODO: consider using std::format from cpp23
# Formatting library
FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.1.1)
set(FMT_INSTALL ON)
# Adds fmt::fmt
FetchContent_MakeAvailable(fmtlib)

# Eigen
FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)
set(EIGEN_BUILD_DOC OFF)
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
FetchContent_MakeAvailable(Eigen)

find_package (Eigen3 3.4 REQUIRED NO_MODULE)

# Fast float
FetchContent_Declare(
        fast_float
        GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
        GIT_TAG tags/v6.0.0
        GIT_SHALLOW TRUE)
set(FASTFLOAT_INSTALL OFF)
# Adds fast_float
FetchContent_MakeAvailable(fast_float)

# Fast unordered map and set
FetchContent_Declare(
        unordered_dense
        GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
        GIT_TAG tags/v4.3.1
        GIT_SHALLOW TRUE)
# Adds unordered_dense::unordered_dense
FetchContent_MakeAvailable(unordered_dense)

# SIMD vector operations
FetchContent_Declare(
        vectorclass
        GIT_REPOSITORY https://github.com/vectorclass/version2.git
        GIT_TAG tags/v2.02.01)
FetchContent_MakeAvailable(vectorclass)

# Fast hash Functions
FetchContent_Declare(
        xxhash
        GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
        GIT_TAG tags/v0.8.2
        GIT_SHALLOW TRUE)
set(XXHASH_BUNDLED_MODE ON)
set(XXH_INLINE_ALL ON)
# Adds xxhash
FetchContent_MakeAvailable(xxHash)
file(GLOB XXHASH_SOURCES ${xxhash_SOURCE_DIR}/*.c)
list(REMOVE_ITEM XXHASH_SOURCES ${xxhash_SOURCE_DIR}/xxhsum.c)
list(REMOVE_ITEM XXHASH_SOURCES ${xxhash_SOURCE_DIR}/xxh_x86dispatch.c)
add_library(xxhash STATIC ${XXHASH_SOURCES})
target_include_directories(xxhash PUBLIC ${xxhash_SOURCE_DIR})

# EVE vector engine
#TODO: setup eve like this [https://github.com/STEllAR-GROUP/hpx/blob/c374f26626038f020dd12f842d4dc5d67d02f59d/cmake/HPX_SetupEve.cmake#L31]
FetchContent_Declare(
        eve
        GIT_REPOSITORY https://github.com/jfalcou/eve.git
        GIT_TAG tags/v2023.02.15
        GIT_SHALLOW TRUE)
set(EVE_BUILD_DOCUMENTATION OFF)
set(EVE_BUILD_TEST OFF)
FetchContent_MakeAvailable(eve)
##############################################
# Create target and set properties

file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/turing_forge/*/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/vstat/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/csv/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/ceres/*.h"
)

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*/*.cpp"
)

add_library(turingforge
        ${SRC_LIST}
        ${HEADER_LIST}
)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(Turingforge::turingforge ALIAS turingforge)

#Set target properties
target_include_directories(turingforge
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            ${PROJECT_SOURCE_DIR}/include
            ${vectorclass_SOURCE_DIR}
)

# TODO: put -Werror in there again once the fmtlib tautological bug is fixed (C++ is 90% busy work for large projects)
target_compile_features(turingforge PRIVATE cxx_auto_type)
target_compile_options(turingforge PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Werror -pedantic>)

target_link_libraries(turingforge
        PUBLIC
            fmt::fmt Eigen3::Eigen unordered_dense::unordered_dense
        PRIVATE
            xxhash fast_float eve::eve
)

##############################################
# Installation instructions

# TODO: fix libraries not being in any export set
option(TURINGFORGE_INSTALL "Enable install" OFF)
set(TURINGFORGE_INSTALL OFF)

if (TURINGFORGE_INSTALL)
    include(GNUInstallDirs)
    set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Turingforge)

    install(TARGETS turingforge
            EXPORT turingforge-targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # This is required so that the exported target has the name Turingforge and not turingforge
    set_target_properties(turingforge PROPERTIES EXPORT_NAME Turingforge)

    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # Export the targets to a script
    install(EXPORT turingforge-targets
            FILE
                TuringforgeTargets.cmake
            NAMESPACE
                Turingforge::
            DESTINATION
                ${INSTALL_CONFIGDIR}
    )

    # Create a ConfigVersion.cmake file
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

    # Install the config, configversion and custom find modules if any
    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeConfigVersion.cmake
            DESTINATION ${INSTALL_CONFIGDIR}
    )

    ##############################################
    ## Exporting from the build tree
    #configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/SOMECUSTOM.cmake
    #        ${CMAKE_CURRENT_BINARY_DIR}/SOMECUSTOM.cmake
    #        COPYONLY)

    export(EXPORT turingforge-targets
            FILE ${CMAKE_CURRENT_BINARY_DIR}/TuringforgeTargets.cmake
            NAMESPACE Turingforge::)

    # Register package in the User Package Registry
    export(PACKAGE Turingforge)

    # Make sure IDEs put the headers in a nice place
    source_group(
            TREE "${PROJECT_SOURCE_DIR}/include"
            PREFIX "Header Files"
            FILES ${HEADER_LIST}
    )
endif()
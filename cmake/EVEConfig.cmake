# -*- mode: cmake -*-

option(TURINGFORGE_ENABLE_EVE "Build EVE library from source" ON)

if (NOT TARGET eve::eve)
    # compatibility with older CMake versions
    if (EVE_ROOT AND NOT Eve_ROOT)
        set(Eve_ROOT ${EVE_ROOT} CACHE PATH "Eve base directory")
        unset(EVE_ROOT CACHE)
    endif ()

    if (TURINGFORGE_ENABLE_EVE)
        message(STATUS "Building EVE from source...")

        set(EVE_VERSION "2023.2.15")
        set(EVE_VERSION_TAG "2023.02.15")

        # Define EVE source paths and options
        set(EVE_URL "https://github.com/jfalcou/eve.git" CACHE STRING "EVE repository URL")
        set(EVE_TAG "tags/v${EVE_VERSION_TAG}" CACHE STRING "EVE version tag")
        set(EVE_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/eve-src" CACHE PATH "Path to EVE source directory")
        set(EVE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/eve-build" CACHE PATH "Path to EVE build directory")
        set(EVE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/eve-install" CACHE PATH "Path to install EVE")

        ExternalProject_Add(eve-external
                PREFIX "${CMAKE_CURRENT_BINARY_DIR}/eve-prefix"
                STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/eve/stamp
                GIT_REPOSITORY ${EVE_URL}
                GIT_TAG ${EVE_TAG}
                SOURCE_DIR ${EVE_SOURCE_DIR}
                BINARY_DIR ${EVE_BINARY_DIR}
                INSTALL_DIR ${EVE_INSTALL_PREFIX}
                CMAKE_ARGS
                -DCMAKE_INSTALL_PREFIX=${EVE_INSTALL_PREFIX}
                -DCMAKE_BUILD_TYPE=Release
                -DEVE_BUILD_DOCUMENTATION=OFF    # Disable documentation build
                -DEVE_BUILD_TEST=OFF             # Disable testing
                -DEVE_BUILD_BENCHMARKS=OFF       # Disable pkg-config
        )

        add_library(eve INTERFACE)
        add_dependencies(eve eve-external)
        target_include_directories(eve SYSTEM INTERFACE
                $<BUILD_INTERFACE:${EVE_INSTALL_PREFIX}/include/eve-${EVE_VERSION}>
                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

        install(
                TARGETS eve
                EXPORT TuringforgeEveTarget
                COMPONENT core
        )

        install(
                DIRECTORY ${EVE_INSTALL_PREFIX}/include/
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                COMPONENT core
                FILES_MATCHING
                PATTERN "*.hpp"
                PATTERN "*.ipp"
        )

        export(
                TARGETS eve
                NAMESPACE eve::
                FILE "${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${TURINGFORGE_PACKAGE_NAME}/TuringforgeEveTarget.cmake"
        )

        install(
                EXPORT TuringforgeEveTarget
                NAMESPACE eve::
                FILE TuringforgeEveTarget.cmake
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TURINGFORGE_PACKAGE_NAME}
                COMPONENT cmake
        )

        add_library(eve::eve ALIAS eve)

        target_link_libraries(turingforge PUBLIC eve::eve)

    else ()
        if (Eve_ROOT)
            find_package(eve REQUIRED PATHS ${Eve_ROOT})
            target_link_libraries(turingforge PUBLIC eve::eve)
        else ()
            message(FATAL_ERROR "Eve_ROOT not set and TURINGFORGE_ENABLE_EVE is OFF")
        endif()
    endif ()
endif ()
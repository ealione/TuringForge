# ---- Developer mode ----

# Developer mode enables targets and code paths in the CMake scripts that are
# only relevant for the developer(s) of operon
# Targets necessary to build the project must be provided unconditionally, so
# consumers can trivially build and package the project
if(PROJECT_IS_TOP_LEVEL)
    option(TURINGFORGE_DEVELOPER_MODE "Enable developer mode" OFF)
    option(BUILD_SHARED_LIBS "Build shared libs." OFF)

    # compile and link-time options
    set(JEMALLOC_DESCRIPTION             "Link against jemalloc, a general purpose malloc(3) implementation that emphasizes fragmentation avoidance and scalable concurrency support [default=OFF].")
    set(USE_SINGLE_PRECISION_DESCRIPTION "Perform model evaluation using floats (single precision) instead of doubles. Great for reducing runtime, might not be appropriate for all purposes [default=OFF].")
    set(USE_CERES_DESCRIPTION            "Use the non-linear least squares optimizer from Ceres solver to tune model coefficients (if OFF, Eigen::LevenbergMarquardt will be used instead).")
    set(MATH_BACKEND_DESCRIPTION         "Math library for tree evaluation (defaults to Eigen)")

    # option descriptions
    option(USE_JEMALLOC         ${JEMALLOC_DESCRIPTION}             OFF)
    option(USE_SINGLE_PRECISION ${USE_SINGLE_PRECISION_DESCRIPTION}  ON)
    option(USE_CERES            ${USE_CERES_DESCRIPTION}            OFF)
    option(MATH_BACKEND         ${MATH_BACKEND_DESCRIPTION}      "Eigen")

    # provide a summary of configured options
    include(FeatureSummary)
    add_feature_info(USE_JEMALLOC         USE_JEMALLOC             ${JEMALLOC_DESCRIPTION})
    add_feature_info(USE_SINGLE_PRECISION USE_SINGLE_PRECISION     ${USE_SINGLE_PRECISION_DESCRIPTION})
    add_feature_info(USE_CERES            USE_CERES                ${USE_CERES_DESCRIPTION})
    add_feature_info(MATH_BACKEND         MATH_BACKEND_DESCRIPTION ${MATH_BACKEND_DESCRIPTION})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
    if(CMAKE_EXPORT_COMPILE_COMMANDS)
        set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    endif()
endif()

# ---- Warning guard ----

# target_include_directories with the SYSTEM modifier will request the compiler
# to omit warnings from the provided paths, if the compiler supports that
# This is to provide a user experience similar to find_package when
# add_subdirectory or FetchContent is used to consume this project
set(turingforge_warning_guard "")
if(NOT PROJECT_IS_TOP_LEVEL)
    option(
            TURINGFORGE_INCLUDES_WITH_SYSTEM
            "Use SYSTEM modifier for Turingforge's includes, disabling warnings"
            ON
    )
    mark_as_advanced(TURINGFORGE_INCLUDES_WITH_SYSTEM)
    if(TURINGFORGE_INCLUDES_WITH_SYSTEM)
        set(turingforge_warning_guard SYSTEM)
    endif()
endif()
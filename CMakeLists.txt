cmake_minimum_required(VERSION 3.25)

#if(POLICY CMP0068)
#    cmake_policy(SET CMP0068 NEW)
#    set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
#endif()
#
#if(POLICY CMP0075)
#    cmake_policy(SET CMP0075 NEW)
#endif()
#
#if(POLICY CMP0077)
#    cmake_policy(SET CMP0077 NEW)
#endif()

# CMP0116: Ninja generators transform `DEPFILE`s from `add_custom_command()`
# New in CMake 3.20. https://cmake.org/cmake/help/latest/policy/CMP0116.html
#if(POLICY CMP0116)
#    cmake_policy(SET CMP0116 OLD)
#endif()

# Version info
set(TURINGFORGE_VERSION_MAJOR 0)
set(TURINGFORGE_VERSION_MINOR 1)
set(TURINGFORGE_VERSION_PATCH 0)
set(TURINGFORGE_VERSION ${TURINGFORGE_VERSION_MAJOR}.${TURINGFORGE_VERSION_MINOR}.${TURINGFORGE_VERSION_PATCH})

# Project Info
set(PACKAGE_NAME          "TuringForge")
set(PACKAGE_VERSION       "${TURINGFORGE_VERSION}-dev")
set(PACKAGE_VERSION_HEX   "0x00010000")
set(PACKAGE_STRING        "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME       "${PACKAGE_NAME}-${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT     "https://github.com/ealiaj/TuringForge/issues/")

# Default build type for single-config generators (set this before project() command)
# For multi-config generators, such as Visual Studio, use: cmake --build . --config=<CONFIG>
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE
            STRING "Build type for single-config generators" FORCE)
endif()

project(${PACKAGE_NAME} LANGUAGES CXX DESCRIPTION "Hybrid Hardware/Software Symbolic Regression Library")

# Avoid use of compiler language extensions, i.e. -std=c++14 not -std=gnu++14
if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS FALSE)
endif()

# Set the default warning level we use to pass the GitHub workflows
if (TF_ENABLE_DEFAULT_WARNING_LEVEL)
    if (MSVC)
        # warning level 3
        # There are also specific warnings disabled for MSCV in cmake/Platform.cmake.
        add_compile_options(-W3)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        # lots of warnings
        add_compile_options(-Wall -Wno-unknown-pragmas)
    endif()
endif()

# Treat warnings on compile as errors
if (TF_COMPILE_WARNING_AS_ERROR)
    # all compiler warnings as errors
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        # since v3.24 cmake can set all compiler warnings as errors itself
        set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
    else()
        # or set them manually
        if (MSVC)
            add_compile_options(-WX)
        elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
            add_compile_options(-Werror)
        endif()
    endif()
endif()

include(CheckCXXSourceCompiles)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# TuringForge project.
set(TURINGFORGE_MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} ) # --src-root
set(TURINGFORGE_MAIN_INCLUDE_DIR ${TURINGFORGE_MAIN_SRC_DIR}/include)

set(TURINGFORGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TURINGFORGE_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(TURINGFORGE_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(TURINGFORGE_LIBRARY_DIR ${CMAKE_BINARY_DIR}/lib)
set(TURINGFORGE_TOOLS_DIR ${CMAKE_BINARY_DIR}/bin)
set(TURINGFORGE_UTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)

# Add project files to the include path
include_directories(${TURINGFORGE_MAIN_INCLUDE_DIR})
include_directories(${TURINGFORGE_INCLUDE_DIR})

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(test)

add_executable(turing-forge TuringForge.cpp include/turingforge/AdaptiveParsimony.h include/turingforge/Constants.h include/turingforge/Options.h include/turingforge/Configure.h include/turingforge/Complexity.h include/turingforge/OptionsStructure.h include/turingforge/OperatorEnum.h include/turingforge/Optim.h include/turingforge/Loss/Weighted.h include/turingforge/Loss/Traits.h include/turingforge/Loss/LossFunctions.h include/turingforge/Loss/Scaled.h include/turingforge/Utils.h include/turingforge/Loss/Margin.h include/turingforge/Loss/Other.h include/turingforge/Loss/Distance.h include/turingforge/Loss/Utils.h)
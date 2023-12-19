cmake_minimum_required(VERSION 3.25)
project(host_code)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -pedantic -Werror)

project(
  Turingforge
  VERSION 0.0.1
  DESCRIPTION "Hardware powered Symbolic Regression library."
  HOMEPAGE_URL "https://"
  LANGUAGES CXX
)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(PROJECT_IS_TOP_LEVEL)

  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

  # Ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

  # Docs only available if this is the main app
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

include(FetchContent)
include(ExternalProject)

find_package(MPI REQUIRED)

# Formatting library
FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 5.3.0)
# Adds fmt::fmt
FetchContent_MakeAvailable(fmtlib)

add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)

# Testing only available if this is the main app
# Emergency override TURINGFORGE_CMAKE_BUILD_TESTING provided as well
if((PROJECT_IS_TOP_LEVEL OR TURINGFORGE_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
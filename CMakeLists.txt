cmake_minimum_required(VERSION 3.19...3.25 FATAL_ERROR)


include(cmake/Prelude.cmake)

project(
        Turingforge
        VERSION 0.1.0
        DESCRIPTION "Hardware powered Symbolic Regression library."
        HOMEPAGE_URL "https://"
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(DEPRECATION ON)
set(EXPORT_COMPILE_COMMANDS ON)

set(TURINGFORGE_PACKAGE_NAME "TuringForge")

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Variables.cmake)

# ---- Attempt to get revision information from git ----
find_package(Git)     # retrieve revision number for version info
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
  include(cmake/get-git-revision.cmake)
  EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD OUTPUT_VARIABLE SHORT_SHA OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(REVISION ${SHORT_SHA} CACHE STRING "git short sha" FORCE)

  # only use the plugin to tie the configure state to the sha to force rebuilds
  # of files that depend on version.h
  get_git_head_revision(REFSPEC COMMITHASH.cmake)
else()
  message(WARNING "Git not found, cannot set version info")
  SET(REVISION "unknown")
endif()

option(TURINGFORGE_BUILD_TESTING "Build tests" ON)
option(TURINGFORGE_INSTALL "Enable install" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Make sure that custom modules are found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

##############################################
# Add subdirectories
add_subdirectory(src)

# print summary of enabled/disabled features
feature_summary(WHAT ENABLED_FEATURES DESCRIPTION "Enabled features:" QUIET_ON_EMPTY)
feature_summary(WHAT DISABLED_FEATURES DESCRIPTION "Disabled features:" QUIET_ON_EMPTY)

include(GenerateExportHeader)
generate_export_header(
        turingforge
        BASE_NAME turingforge
        EXPORT_FILE_NAME export/turingforge/turingforge_export.hpp
        CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

# ---- Timestamp the current build ----
string(TIMESTAMP OPERON_BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%SZ")

# ---- Add build information ----
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/turing_forge/core/buildinfo.hpp.in
        ${CMAKE_BINARY_DIR}/buildinfo.hpp)

# Testing only available if this is the main app
# Emergency override TURINGFORGE_CMAKE_BUILD_TESTING provided as well
if ((PROJECT_IS_TOP_LEVEL OR TURINGFORGE_CMAKE_BUILD_TESTING) AND TURINGFORGE_BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

##############################################
# Packaging

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(PROJECT_IS_TOP_LEVEL)
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

  # Examples
  option(BUILD_EXAMPLES "Build examples tree." ${TURINGFORGE_DEVELOPER_MODE})
  # TODO: why do we need to force it here?
  set(BUILD_EXAMPLES ${TURINGFORGE_DEVELOPER_MODE} CACHE BOOL "Build examples tree." FORCE)
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()

  # CLI
  option(BUILD_CLI_PROGRAMS "Build command-line programs." TRUE)
  if (BUILD_CLI_PROGRAMS)
    add_subdirectory(cli)
  endif()

  # Installation
  if (TURINGFORGE_INSTALL)
    include(Packaging)
  endif()
endif()
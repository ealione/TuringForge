cmake_minimum_required(VERSION 3.19...3.25 FATAL_ERROR)

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

option(TURINGFORGE_BUILD_TESTING "Build tests" ON)
option(TURINGFORGE_INSTALL "Enable install" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#Make sure that custom modules are found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

##############################################
# Add subdirectories
add_subdirectory(src)

# The executable code is here
#add_subdirectory(apps)

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

  # Installation
  if (TURINGFORGE_INSTALL)
    include(Packaging)
  endif()
endif()
##############################################
# Find/Install system dependencies

include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)

# TODO: consider using std::format from cpp23
# Formatting library
FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.1.1)
set(FMT_INSTALL ON)
# Adds fmt::fmt
FetchContent_MakeAvailable(fmtlib)

# EVE vector engine
include(EVEConfig)

# Eigen
include(EigenConfig)

# Fast float
FetchContent_Declare(
        fast_float
        GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
        GIT_TAG tags/v6.0.0
        GIT_SHALLOW TRUE)
set(FASTFLOAT_INSTALL OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
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

# MDSpan
find_package(mdspan QUIET)
if (NOT mdspan_FOUND)
    message(STATUS "No installed mdspan found, fetching from Github")
    include(FetchContent)
    FetchContent_Declare(
            mdspan
            GIT_REPOSITORY https://github.com/kokkos/mdspan.git
            GIT_TAG        stable
    )
    FetchContent_MakeAvailable(mdspan)
    FetchContent_GetProperties(mdspan)
    if(NOT mdspan_POPULATED)
        FetchContent_Populate(mdspan)
        add_subdirectory(${mdspan_SOURCE_DIR} ${mdspan_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif()

# Sorter vector engine
FetchContent_Declare(
        cpp-sort
        GIT_REPOSITORY https://github.com/Morwenn/cpp-sort.git
        GIT_TAG 1.15.0
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cpp-sort)

# taskflow
FetchContent_Declare(
        taskflow
        GIT_REPOSITORY https://github.com/taskflow/taskflow.git
        GIT_TAG        v3.6.0
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
)
FetchContent_GetProperties(taskflow)
if(NOT taskflow_POPULATED)
    FetchContent_Populate(taskflow)
endif()
set(TF_BUILD_TESTS OFF CACHE BOOL "Enables build of tests")
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "Enables build of examples")

FetchContent_MakeAvailable(taskflow)

add_library(taskflow INTERFACE)
target_include_directories(taskflow INTERFACE ${taskflow_SOURCE_DIR})
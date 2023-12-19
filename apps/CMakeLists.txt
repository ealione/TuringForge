# Input Parsing library
FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_SHALLOW TRUE)
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(CXXOPTS_ENABLE_WARNINGS OFF CACHE BOOL "" FORCE)
# Adds cxxopts
FetchContent_MakeAvailable(cxxopts)

add_executable(turingforge main.cpp)

target_compile_features(turingforge PRIVATE cxx_std_20)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

target_link_libraries(turingforge PRIVATE optimizer_lib fmt::fmt cxxopts ${MPI_C_LIBRARIES})
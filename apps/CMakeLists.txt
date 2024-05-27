##############################################
# Find/Install system dependencies

# TODO: Figure out how to install MPI using `FetchContent`
find_package(MPI 3.1)
if(NOT MPI_FOUND)
    message(STATUS "MPI with version >= 3.1 not Found, Preparing OpenMPI ...")
    FetchContent_Declare(openmpi URL https://download.open-mpi.org/release/open-mpi/v4.0/openmpi-4.0.3.tar.gz)
    message(STATUS "Fetching openmpi source code ...")
    FetchContent_MakeAvailable(openmpi)
    set(MPI_CXX_LIBRARIES ${openmpi_BINARY_DIR}/lib/libmpi.so)
else ()
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})

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

# Add Turingforge
find_package(Turingforge 0.1 REQUIRED)

##############################################
# Find/Install system dependencies

add_executable(turingforge-app main.cpp)

target_compile_features(turingforge-app PRIVATE cxx_auto_type)

target_link_libraries(turingforge PRIVATE Turingforge::Turingforge cxxopts ${MPI_C_LIBRARIES})
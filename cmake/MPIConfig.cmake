# -*- mode: cmake -*-

# Check if we want to enable MPI
option(TURINGFORGE_ENABLE_MPI "Enable Eigen support" ON)

if(TURINGFORGE_ENABLE_MPI)
    # Try to find MPI
    find_package(MPI REQUIRED)

    if(MPI_FOUND)
        message(STATUS "MPI found: ${MPI_C_COMPILER} and ${MPI_CXX_COMPILER}")

        # If MPI is found, we set the necessary include directories and link libraries.
        set(MPI_INCLUDE_DIR ${MPI_C_INCLUDE_PATH})
        add_library(mpi INTERFACE)
        target_include_directories(mpi INTERFACE ${MPI_INCLUDE_DIR})
        target_link_libraries(mpi INTERFACE MPI::MPI_C MPI::MPI_CXX)

    else()
        message(STATUS "MPI not found, downloading and building MPICH from source...")

        # Set default paths for MPICH
        # This will not overwrite user defined valuse.
        set(MPICH_URL "http://www.mpich.org/static/downloads/3.1.4/mpich-3.1.4.tar.gz" CACHE STRING
                "Path to the MPICH source tar ball")
        set(MPICH_DOWNDLOAD_DIR "${PROJECT_BINARY_DIR}/mpich/" CACHE PATH
                "Path to MPICH download directory")
        set(MPICH_SOURCE_DIR "${PROJECT_BINARY_DIR}/mpich/source/" CACHE PATH
                "Path to install MPICH")
        set(MPICH_BUILD_DIR "${PROJECT_BINARY_DIR}/mpich/build/" CACHE PATH
                "Path to install MPICH")
        set(MPICH_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
                "Path to install MPICH")

        ExternalProject_Add(mpich
                PREFIX "${MPICH_INSTALL_PREFIX}"
                STAMP_DIR "${PROJECT_BINARY_DIR}/stamp"
                #--Download step--------------
                DOWNLOAD_DIR "${MPICH_DOWNDLOAD_DIR}"
                URL "${MPICH_URL}"
                #--Configure step-------------
                SOURCE_DIR "${MPICH_SOURCE_DIR}"
                CONFIGURE_COMMAND "${MPICH_SOURCE_DIR}/configure"
                "--prefix=${MPICH_INSTALL_PREFIX}"
                "${ENABLE_SHARED}"
                "${ENABLE_STATIC}"
                "--enable-threads=multiple"
                "--enable-cxx"
                "--disable-fortran"
                "CC=${CMAKE_C_COMPILER}"
                "CXX=${CMAKE_CXX_COMPILER}"
                "CFLAGS=${CFLAGS}"
                "CXXFLAGS=${CXXFLAGS}"
                "LDFLAGS=${LDFLAGS}"
                #--Build step-----------------
                BINARY_DIR ${MPICH_BUILD_DIR}
                #--Install step---------------
                INSTALL_DIR "${MPICH_INSTALL_PREFIX}"
        )

        # Set MPICH compilers and create mpi target
        set(MPI_C_COMPILER "${MPICH_INSTALL_PREFIX}/bin/mpicc" CACHE STRING "MPICH C compiler")
        set(MPI_CXX_COMPILER "${MPICH_INSTALL_PREFIX}/bin/mpicxx" CACHE STRING "MPICH C++ compiler")
        add_library(mpi INTERFACE)
        target_include_directories(mpi INTERFACE "${MPICH_INSTALL_PREFIX}/include")
        target_link_libraries(mpi INTERFACE "${MPICH_INSTALL_PREFIX}/lib/libmpi.so")

        # Ensure turingforge depends on mpich
        add_dependencies(turingforge mpich)
        target_link_libraries(turingforge PUBLIC mpi)
    endif()
endif()
# -*- mode: cmake -*-

# Check if we want to enable Eigen
option(TURINGFORGE_ENABLE_EIGEN "Enable Eigen support" ON)

# Check for Eigen
if(TURINGFORGE_ENABLE_EIGEN)
    find_package(Eigen 3.4 QUIET)

    if(EIGEN_FOUND)
        message(STATUS "Eigen3 found: ${Eigen3_VERSION}")

        list(APPEND CMAKE_REQUIRED_INCLUDES ${EIGEN_INCLUDE_DIR})

        # Perform a compile check with Eigen
        include(CheckCXXSourceCompiles)
        cmake_push_check_state()

        set(CMAKE_REQUIRED_LIBRARIES Eigen3::Eigen)
        CHECK_CXX_SOURCE_COMPILES("
            #include <Eigen/Core>
            #include <Eigen/Dense>
            #include <iostream>
            int main() {
                Eigen::MatrixXd m = Eigen::MatrixXd::Random(5, 5);
                m = m.transpose() + m;
                Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(m);
                Eigen::MatrixXd m_invsqrt = eig.operatorInverseSqrt();
                std::cout << m_invsqrt << std::endl;
            }" EIGEN_COMPILES)

        cmake_pop_check_state()

        if (NOT EIGEN_COMPILES)
            message(FATAL_ERROR "Eigen found at ${EIGEN_ROOT}, but could not compile test program")
        endif()

        # Link Eigen to turingforge
        target_link_libraries(turingforge PUBLIC Eigen3::Eigen)

    else()
        # If Eigen3 is not found, use ExternalProject_Add to build it from source
        message(STATUS "Eigen3 not found, downloading and building from source...")

        # Create a cache entry for Eigen build variables.
        # Note: This will not overwrite user specified values.
        set(EIGEN_SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/eigen-src" CACHE PATH "Path to Eigen source")
        set(EIGEN_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/eigen-install" CACHE PATH "Path to install Eigen")
        set(EIGEN_URL "https://gitlab.com/libeigen/eigen" CACHE STRING "Path to the Eigen repository")
        set(EIGEN_TAG "3147391d" CACHE STRING "The Eigen revision tag")

        ExternalProject_Add(eigen3
                PREFIX ${CMAKE_CURRENT_BINARY_DIR}/eigen
                STAMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/eigen/stamp
                TIMEOUT 30
                LOG_DOWNLOAD ON
                LOG_CONFIGURE ON
                LOG_BUILD ON
                #--Download step--------------
                GIT_REPOSITORY ${EIGEN_URL}
                GIT_TAG ${EIGEN_TAG}
                #--Configure step-------------
                SOURCE_DIR ${EIGEN_SOURCE_DIR}
                CMAKE_ARGS
                -DCMAKE_INSTALL_PREFIX:path=${EIGEN_INSTALL_DIR}
                -DCMAKE_BUILD_TYPE:STRING=Release
                -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF
                -DEIGEN_BUILD_DOC=OFF                # Disable documentation build
                -DEIGEN_BUILD_TESTING=OFF            # Disable testing
                -DEIGEN_BUILD_PKGCONFIG=OFF          # Disable pkg-config
                BUILD_BYPRODUCTS ${EIGEN_INSTALL_DIR}/include/eigen3/Eigen/Core
                #--Build step-----------------
                BINARY_DIR ${EIGEN_BINARY_DIR}       # Specify build dir location
                BUILD_COMMAND ""  # Eigen is header-only, no need to build
                UPDATE_COMMAND ""  # Skip update step
                #--Install step---------------
                INSTALL_COMMAND ""
                INSTALL_DIR ${EIGEN_INSTALL_DIR} # Installation prefix
                #--Custom targets-------------
        )

        ExternalProject_Get_Property(eigen3 SOURCE_DIR)
        set(EIGEN_INCLUDE_DIR ${SOURCE_DIR} CACHE PATH "Path to Eigen include directory")
        add_library(Eigen3::Eigen INTERFACE IMPORTED GLOBAL)
        add_dependencies(Eigen3::Eigen eigen3)
        target_include_directories(Eigen3::Eigen INTERFACE ${EIGEN_INCLUDE_DIR})
        set_target_properties(Eigen3::Eigen PROPERTIES EXCLUDE_FROM_ALL TRUE)

        add_dependencies(turingforge Eigen3::Eigen)
        target_link_libraries(turingforge PUBLIC Eigen3::Eigen)

    endif()
endif()
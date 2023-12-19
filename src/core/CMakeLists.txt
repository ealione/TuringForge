FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)
set(EIGEN_BUILD_DOC OFF)
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
FetchContent_MakeAvailable(Eigen)

find_package (Eigen3 3.4 REQUIRED NO_MODULE)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/turing_forge/core/*.hpp")
file(GLOB SRC_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/core/*.cpp")

add_library(core_lib ${SRC_LIST} ${HEADER_LIST})

target_include_directories(core_lib PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_compile_features(core_lib PRIVATE cxx_std_20)

target_link_libraries(core_lib PRIVATE fmt::fmt Eigen3::Eigen)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

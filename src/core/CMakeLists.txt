file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/turing_forge/core/*.hpp")
file(GLOB SRC_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/core/*.cpp")

add_library(core_lib ${SRC_LIST} ${HEADER_LIST})

target_include_directories(core_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_compile_features(core_lib PUBLIC cxx_std_20)

target_link_libraries(core_lib PUBLIC fmt::fmt unordered_dense::unordered_dense)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

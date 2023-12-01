file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/turing_forge/*/*.hpp")

add_library(optimizer_lib simulated_annealing.cpp ${HEADER_LIST})

target_include_directories(optimizer_lib PUBLIC ../../include)

#target_link_libraries(optimizer_lib PRIVATE Boost::boost)

target_compile_features(optimizer_lib PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

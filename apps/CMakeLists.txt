add_executable(turingforge main.cpp)

target_compile_features(turingforge PRIVATE cxx_std_20)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

target_link_libraries(turingforge PRIVATE optimizer_lib fmt::fmt cxxopts ${MPI_C_LIBRARIES})
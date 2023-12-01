add_executable(turingforge main.cpp)

target_compile_features(turingforge PRIVATE cxx_std_20)

target_link_libraries(turingforge PRIVATE optimizer_lib fmt::fmt)


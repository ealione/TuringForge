cmake_minimum_required(VERSION 3.19)

project(TuringforgeExamples CXX)

if(PROJECT_IS_TOP_LEVEL)
    find_package(operon REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
    add_executable("${NAME}" "${NAME}.cpp")
    target_link_libraries("${NAME}" PRIVATE Turingforge::turingforge)
    target_compile_features("${NAME}" PRIVATE cxx_std_17)
    add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
    add_dependencies("run_${NAME}" "${NAME}")
    add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(empty_example)
add_example(simple_gp)
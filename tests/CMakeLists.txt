# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.0)
# Adds Catch2::Catch2
FetchContent_MakeAvailable(catch)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

set(TEST_SOURCES
        test_evaluation.cpp
        test_individual.cpp
        test_initialization.cpp
        test_selection.cpp
        test_dispatch_table.cpp
        test_simulated_annealing.cpp
        test_mutation.cpp
        test_error_metrics.cpp
        test_nondominantsort.cpp
        test_regression.cpp
)

string(REPLACE ".cpp" "" BASENAMES "${TEST_SOURCES}")
set(TARGETS ${BASENAMES})

foreach(name ${TARGETS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} turingforge Catch2::Catch2WithMain)
    target_compile_features(${name} PRIVATE cxx_std_20)
endforeach()

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(${TARGETS})
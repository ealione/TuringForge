# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.0)
# Adds Catch2::Catch2
FetchContent_MakeAvailable(catch)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

add_executable(test-expression test_expression.cpp)

target_compile_features(test-expression PRIVATE cxx_std_20)

target_link_libraries(test-expression PRIVATE optimizer_lib Catch2::Catch2WithMain)

# Register a test, then ctest and make test will run it.
add_test(NAME testexpression COMMAND test-expression)

include(Catch)
catch_discover_tests(test-expression)
# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6)
FetchContent_MakeAvailable(catch)
# Adds Catch2::Catch2

add_executable(test-simulated-annealing test_simulated_annealing.cpp)

target_compile_features(test-simulated-annealing PRIVATE cxx_std_20)
target_link_libraries(test-simulated-annealing PRIVATE optimizers_lib Catch2::Catch2)

# Register a test, then ctest and make test will run it.
add_test(NAME testlibtest COMMAND testlib) # Command can be a target


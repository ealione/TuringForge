add_executable(test_loss properties.cpp core.cpp)
target_link_libraries(test_loss PRIVATE Catch2::Catch2WithMain)
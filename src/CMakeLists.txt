##############################################
# Source and Header Files
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/turing_forge/*/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/vstat/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/csv/*.hpp"
        "${PROJECT_SOURCE_DIR}/include/ceres/*.h"
)

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*/*.cpp"
)

##############################################
# Create the turingforge library target
add_library(turingforge ${SRC_LIST} ${HEADER_LIST})

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(Turingforge::turingforge ALIAS turingforge)

##############################################
# Include Directories
target_include_directories(turingforge
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            ${PROJECT_SOURCE_DIR}/include
            ${vectorclass_SOURCE_DIR}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
)

##############################################
# Dependencies
include(Dependencies)

##############################################
# Set Compiler Flags
include(CompilerFlags)

# Apply compiler flags
set_compiler_flags(turingforge)

##############################################
# Optional Address Sanitizer (ASan) Support
option(TURINGFORGE_USE_ASAN "Build with Address Sanitizer" OFF)

if(TURINGFORGE_USE_ASAN)
    message("Building with Address Sanitizer checks!")
    target_compile_options(turingforge PUBLIC -fsanitize=thread -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(turingforge PUBLIC -fsanitize=thread -fsanitize=address)
endif()

# Enable modern C++ features
target_compile_features(turingforge PRIVATE cxx_auto_type)

##############################################
# Link Libraries
target_link_libraries(turingforge
        PUBLIC
            fmt::fmt unordered_dense::unordered_dense std::mdspan libassert::assert taskflow
        PRIVATE
            xxhash fast_float cpp-sort::cpp-sort
)

# Add dependencies to TuringForge
add_dependencies(turingforge eve)

##############################################
# Source Group for IDEs
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})

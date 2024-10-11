cmake_minimum_required(VERSION 3.20)

project(TuringforgeCli LANGUAGES CXX)

if (PROJECT_IS_TOP_LEVEL)
    find_package(turingforge REQUIRED)
endif()

function(add_turingforge_cli NAME)
    add_executable(${NAME}
            ${NAME}.cpp
            operator_factory.cpp
            util.cpp
    )

    target_link_libraries(${NAME} PRIVATE turingforge cxxopts::cxxopts scn::scn)
    target_compile_features(${NAME} PRIVATE cxx_std_20)
    set_target_properties(${NAME} PROPERTIES
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN YES
    )

    target_link_options(${NAME} PRIVATE "-Wl,--no-undefined")

    install(TARGETS ${NAME}
            RUNTIME
            COMPONENT TuringforgeCli_Runtime
            DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endfunction()

add_turingforge_cli(turingforge_gp)
add_turingforge_cli(turingforge_nsgp)
#add_turingforge_cli(turingforge_parse_model)
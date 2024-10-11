# -*- mode: cmake -*-

# Option to enable jemalloc
option(TURINGFORGE_ENABLE_JEMALLOC "Enable jemalloc support" ON)
option(TURINGFORGE_REBUILD_JEMALLOC "Re-build jemalloc from scratch" ON)

# jemalloc integration
if(TURINGFORGE_ENABLE_JEMALLOC)
    set(JEMALLOC_DISABLE_TLS)

    # Determine if we need to build jemalloc from source
    if(${TURINGFORGE_REBUILD_JEMALLOC})
        set(JEMALLOC_PREFIX_DIR "${CMAKE_CURRENT_BINARY_DIR}/jemalloc" CACHE PATH "Path to Jemalloc source")
        set(JEMALLOC_SRC_DIR "${JEMALLOC_PREFIX_DIR}/src/jemalloc")
        set(JEMALLOC_INSTALL_DIR "${JEMALLOC_PREFIX_DIR}/install")

        ExternalProject_Add(jemalloc
                GIT_REPOSITORY https://github.com/jemalloc/jemalloc.git
                GIT_TAG 5.3.0
                LOG_DOWNLOAD ON
                LOG_CONFIGURE ON
                LOG_BUILD ON
                PREFIX ${JEMALLOC_PREFIX_DIR}
                CONFIGURE_COMMAND cd ${JEMALLOC_SRC_DIR} && ./autogen.sh && ./configure --prefix=${JEMALLOC_INSTALL_DIR} --enable-prof ${JEMALLOC_DISABLE_TLS}
                BUILD_COMMAND cd ${JEMALLOC_SRC_DIR} && make install_lib_static install_include
                INSTALL_COMMAND ""
                UPDATE_COMMAND ""
        )

        # Create libjemalloc and libjemalloc_pic targets to be used as
        # dependencies
#        add_library(libjemalloc STATIC IMPORTED GLOBAL)
#        add_library(libjemalloc_pic STATIC IMPORTED GLOBAL)

#        set_property(TARGET libjemalloc PROPERTY IMPORTED_LOCATION ${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a)
#        set_property(TARGET libjemalloc_pic PROPERTY IMPORTED_LOCATION ${JEMALLOC_INSTALL_DIR}/lib/libjemalloc_pic.a)
#
#        link_libraries(-Wl,--no-as-needed)
#        link_libraries(dl ${libjemalloc_pic})

        set(JEMALLOC_SOURCE_DIR ${JEMALLOC_INSTALL_DIR})
        set(JEMALLOC_LIBRARIES ${JEMALLOC_INSTALL_DIR}/lib/libjemalloc_pic.a ${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a)

#        add_library(JeMalloc::JeMalloc STATIC IMPORTED)
#        set_target_properties(JeMalloc::JeMalloc PROPERTIES IMPORTED_LOCATION "${JEMALLOC_LIBRARY}")
#        add_dependencies(JeMalloc::JeMalloc jemalloc)
#        file(MAKE_DIRECTORY ${JEMALLOC_INCLUDE_DIRS})
#        set_property(TARGET JeMalloc::JeMalloc APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${JEMALLOC_INCLUDE_DIRS})

        include_directories(${JEMALLOC_INSTALL_DIR}/include)
#        target_link_libraries(turingforge PUBLIC ${JEMALLOC_INSTALL_DIR}/lib/libjemalloc_pic.a ${JEMALLOC_INSTALL_DIR}/lib/libjemalloc.a)

    else()
        # Find pre-installed jemalloc
        find_path(JEMALLOC_INCLUDE_DIR jemalloc/jemalloc.h)
        find_library(JEMALLOC_LIBRARIES jemalloc)

        if(JEMALLOC_INCLUDE_DIR AND JEMALLOC_LIBRARIES)
            message(STATUS "Found jemalloc")
            include_directories(${JEMALLOC_INCLUDE_DIR})
            target_link_libraries(turingforge PUBLIC ${JEMALLOC_LIBRARIES})
        else()
            message(FATAL_ERROR "jemalloc not found and rebuild option is off.")
        endif()
    endif()
endif()

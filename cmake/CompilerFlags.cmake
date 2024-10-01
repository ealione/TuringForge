function(set_compiler_flags TARGET)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_definitions(${TARGET} PRIVATE
                $<$<CONFIG:Debug>:
                _LIBCPP_DEBUG=0
                _LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG
                _LIBCPP_ENABLE_DEBUG_MODE=1
                _LIBCPP_DEBUG_RANDOMIZE_UNSPECIFIED_STABILITY
                _LIBCPP_DEBUG_STRICT_WEAK_ORDERING_CHECK>
                $<$<CONFIG:Release>:_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE>
        )
        target_compile_options(${TARGET} PRIVATE
                $<$<CONFIG:Debug>:
                -Og -Wall -Wextra -Wpedantic -Wmissing-field-initializers -Wundef -Wcast-align -Wchar-subscripts -Wnon-virtual-dtor -Wunused-local-typedefs -Wpointer-arith -Wwrite-strings -Wformat-security -Wlogical-op -Wenum-conversion -Wdouble-promotion -Wconversion -Wshadow -Wno-psabi -Wno-variadic-macros -Wno-long-long -fno-check-new -fno-common -fstrict-aliasing>
                $<$<CONFIG:Release>:
                -Ofast -DNDEBUG -march=native -mavx2 -mfma>
                $<$<CONFIG:RelWithDebInfo>:
                -g -O2 -march=native -mavx2 -mfma>
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_definitions(${TARGET} PRIVATE
                $<$<CONFIG:Debug>:GLIBCXX_DEBUG>
        )
        target_compile_options(${TARGET} PRIVATE
                $<$<CONFIG:Debug>:
                #-march=x86-64 -mavx2 -mfma -Og -fanalyzer -ggdb -Wall -Wextra -Wpedantic -Wmissing-field-initializers -Wundef -Wcast-align -Wchar-subscripts -Wnon-virtual-dtor -Wunused-local-typedefs -Wpointer-arith -Wwrite-strings -Wformat-security -Wlogical-op -Wenum-conversion -Wdouble-promotion -Wconversion -Wshadow -Wno-psabi -Wno-variadic-macros -Wno-long-long -fno-check-new -fno-common -fstrict-aliasing>
                -march=native -mavx2 -mfma -Og -ggdb -Wall -Wextra -Wpedantic>
                $<$<CONFIG:Release>:
                -Ofast -DNDEBUG -march=native -mavx2 -mfma>
                $<$<CONFIG:RelWithDebInfo>:
                -g -O2 -march=native -mavx2 -mfma>
        )
    endif()
endfunction()
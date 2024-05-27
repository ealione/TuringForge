#include "turing_forge/core/buildinfo.hpp"
#include "turing_forge/core/version.hpp"


namespace Turingforge {

    auto Version() -> std::string {
#if defined(USE_SINGLE_PRECISION)
        constexpr auto precision = "single";
#else
        constexpr auto precision = "double";
#endif
        fmt::memory_buffer buf;// NOLINT
        fmt::format_to(std::back_inserter(buf), "turing_forge rev. {} {} {} {}, timestamp {}\n", TURINGFORGE_REVISION, TURINGFORGE_BUILD, TURINGFORGE_PLATFORM, TURINGFORGE_ARCH, TURINGFORGE_BUILD_TIMESTAMP); // NOLINT
        fmt::format_to(std::back_inserter(buf), "{}-precision build using eigen {}" , precision, TURINGFORGE_EIGEN_VERSION);// NOLINT
        fmt::format_to(std::back_inserter(buf), "\n");
        fmt::format_to(std::back_inserter(buf), "compiler: {} {}, flags: {}\n", TURINGFORGE_COMPILER_ID, TURINGFORGE_COMPILER_VERSION, TURINGFORGE_COMPILER_FLAGS);
        return { buf.begin(), buf.end() };
    }

} // namespace Turingforge
// NOLINTEND
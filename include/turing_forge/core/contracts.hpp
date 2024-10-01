#pragma once

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/ranges.h>


namespace Turingforge {

// NOLINTBEGIN(*)
#ifdef DEBUG
#define EXPECT(cond) \
    if(!(cond)) \
    { \
        fmt::print("Precondition {} failed at {}: {}\n", fmt::format(fmt::fg(fmt::terminal_color::green), "{}", #cond), __FILE__, __LINE__); \
        std::terminate(); \
    }
#else
#define EXPECT(cond) (void)(cond)
#endif

#define ENSURE(cond) \
    if(!(cond)) \
    { \
        fmt::print("Precondition {} failed at {}: {}\n", fmt::format(fmt::fg(fmt::terminal_color::green), "{}", #cond), __FILE__, __LINE__); \
        throw std::runtime_error(fmt::format("Precondition {} failed at {}: {}", #cond, __FILE__, __LINE__)); \
    }
// NOLINTEND(*)

}
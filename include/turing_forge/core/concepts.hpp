#pragma once

#include <concepts>
#include <type_traits>

namespace Turingforge::Concepts {
    // T is an arithmetic number
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;
} // namespace Turingforge::Concepts
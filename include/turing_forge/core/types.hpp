#pragma once

#include <cstddef>
#include <cstdint>

namespace Turingforge
{

    using Hash = uint64_t;

    template <typename T>
    using Vector = std::vector<T>;

    template <typename T>
    using Span = std::span<T>;

#if defined(USE_SINGLE_PRECISION)
    using Scalar = float;
#else
    using Scalar = double;
#endif

}

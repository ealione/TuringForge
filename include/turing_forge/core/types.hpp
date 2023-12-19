#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

#include "constants.hpp"
#include "turing_forge/random/random.hpp"


namespace Turingforge {

    using Hash = uint64_t;

    using RandomGenerator = Random::RomuTrio;

    template <typename T>
    using Vector = std::vector<T>;

    template <typename T>
    using Span = std::span<T>;

    template <class Key, class T>
    using Map = std::unordered_map<Key, T>;

    template<typename T, T... Ints>
    using Seq = std::integer_sequence<T, Ints...>;

#if defined(USE_SINGLE_PRECISION)
    using Scalar = float;
#else
    using Scalar = double;
#endif

}
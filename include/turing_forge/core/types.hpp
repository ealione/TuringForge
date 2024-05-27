#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

#include <ankerl/unordered_dense.h>

#include "constants.hpp"
#include "turing_forge/random/random.hpp"


namespace Turingforge {

    using Hash = uint64_t;
    constexpr HashFunction HashFunc = HashFunction::XXHash;

    using RandomGenerator = Random::RomuTrio;

    template <typename T>
    using Vector = std::vector<T>;

    template <typename T>
    using Span = std::span<T>;

    template <class Key,
            class T,
            class Hash = ankerl::unordered_dense::hash<Key>,
            class KeyEqual = std::equal_to<Key>,
            class AllocatorOrContainer = std::allocator<std::pair<Key, T>>,
            class Bucket = ankerl::unordered_dense::bucket_type::standard>
    using Map = ankerl::unordered_dense::detail::table<Key, T, Hash, KeyEqual, AllocatorOrContainer, Bucket, false>;

    template <class Key,
            class T,
            class Hash = ankerl::unordered_dense::hash<Key>,
            class KeyEqual = std::equal_to<Key>,
            class AllocatorOrContainer = std::allocator<std::pair<Key, T>>,
            class Bucket = ankerl::unordered_dense::bucket_type::standard>
    using SegmentedMap = ankerl::unordered_dense::detail::table<Key, T, Hash, KeyEqual, AllocatorOrContainer, Bucket, true>;

    template <class Key,
            class Hash = ankerl::unordered_dense::hash<Key>,
            class KeyEqual = std::equal_to<Key>,
            class AllocatorOrContainer = std::allocator<Key>,
            class Bucket = ankerl::unordered_dense::bucket_type::standard>
    using Set = ankerl::unordered_dense::detail::table<Key, void, Hash, KeyEqual, AllocatorOrContainer, Bucket, false>;

    template <class Key,
            class Hash = ankerl::unordered_dense::hash<Key>,
            class KeyEqual = std::equal_to<Key>,
            class AllocatorOrContainer = std::allocator<Key>,
            class Bucket = ankerl::unordered_dense::bucket_type::standard>
    using SegmentedSet = ankerl::unordered_dense::detail::table<Key, void, Hash, KeyEqual, AllocatorOrContainer, Bucket, true>;

    template<typename T, T... Ints>
    using Seq = std::integer_sequence<T, Ints...>;

#if defined(USE_SINGLE_PRECISION)
    using Scalar = float;
#else
    using Scalar = double;
#endif

}
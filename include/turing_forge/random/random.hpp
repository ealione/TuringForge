#pragma once

#include <algorithm>
#include <random>
#include <type_traits>

#include "turing_forge/core/types.hpp"
#include "turing_forge/core/node.hpp"
#include "romu.hpp"

namespace Turingforge::Random {

    template<typename R, typename T>
    auto Uniform(R &random, T a, T b) -> T {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
        using Dist = std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>;
        return Dist(a, b)(random);
    }

    template<typename R, typename InputIterator>
    auto Sample(R &random, InputIterator start, InputIterator end) -> InputIterator {
        auto dist = std::distance(start, end);
        if (dist <= 1) {
            return start;
        }
        std::advance(start, Uniform(random, decltype(dist){0}, dist - 1));
        return start;
    }

    template<typename R, typename InputIterator>
    auto Sample(R &random, InputIterator start, InputIterator end,
                std::add_pointer_t<bool(typename InputIterator::value_type const &)> condition) -> InputIterator {
        auto n = std::count_if(start, end, condition);

        if (n == 0) {
            return end; // no element satisfies the condition
        }

        auto m = Uniform(random, decltype(n){0}, n - 1);
        InputIterator it;
        for (it = start; it < end; ++it) {
            if (condition(*it) && 0 == m--) {
                break;
            }
        }

        return it;
    }

    // sample n elements and write them to the output iterator
    template<typename R, typename InputIterator, typename OutputIterator>
    auto Sample(R &random, InputIterator start, InputIterator end, OutputIterator out, size_t n) -> OutputIterator {
        return std::sample(start, end, out, n, random);
    }

    auto RandomSymbol = [](auto &random) {
        auto all_types = Turingforge::NodeTypes::GetAllTypes();
        return *Sample(random, all_types.begin(), all_types.end());
    };
}
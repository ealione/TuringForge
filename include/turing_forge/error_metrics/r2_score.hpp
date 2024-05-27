#pragma once

#include <iterator>
#include <type_traits>
#include <vstat/vstat.hpp>
#include "turing_forge/core/types.hpp"

namespace Turingforge {
template<typename InputIt1, typename InputIt2>
inline auto R2Score(InputIt1 begin1, InputIt1 end1, InputIt2 begin2) noexcept -> double
{
    using V1 = typename std::iterator_traits<InputIt1>::value_type;
    using V2 = typename std::iterator_traits<InputIt2>::value_type;
    static_assert(std::is_arithmetic_v<V1>, "InputIt1: value_type must be arithmetic.");
    static_assert(std::is_arithmetic_v<V2>, "InputIt2: value_type must be arithmetic.");
    static_assert(std::is_same_v<V1, V2>, "The types must be the same");
    using vstat::univariate::accumulate;
    auto sqres = [](auto a, auto b) { auto e=a-b; return e*e; };
    auto const ssr = accumulate<V1>(begin1, end1, begin2, sqres).sum;
    auto const sst = accumulate<V2>(begin2, begin2 + std::distance(begin1, end1)).ssr;
    if (sst < std::numeric_limits<double>::epsilon()) { return std::numeric_limits<double>::lowest(); }
    return 1.0 - ssr / sst;
}

template<typename InputIt1, typename InputIt2, typename InputIt3>
inline auto R2Score(InputIt1 begin1, InputIt1 end1, InputIt2 begin2, InputIt3 begin3) noexcept -> double
{
    using V1 = typename std::iterator_traits<InputIt1>::value_type;
    using V2 = typename std::iterator_traits<InputIt2>::value_type;
    static_assert(std::is_arithmetic_v<V1>, "InputIt1: value_type must be arithmetic.");
    static_assert(std::is_arithmetic_v<V2>, "InputIt2: value_type must be arithmetic.");
    static_assert(std::is_same_v<V1, V2>, "The types must be the same");
    using vstat::univariate::accumulate;
    auto sqres = [](auto a, auto b) { auto e=a-b; return e*e; };
    auto const ssr = accumulate<V1>(begin1, end1, begin2, begin3, sqres).sum;
    auto end2 = begin2 + std::distance(begin1, end1);
    auto const m = accumulate<V2>(begin2, end2).mean;
    auto const sst = accumulate<V2>(begin2, end2, begin3, [&](auto v) { return sqres(v, m); }).sum;
    if (sst < std::numeric_limits<double>::epsilon()) { return std::numeric_limits<double>::lowest(); }
    return 1.0 - ssr / sst;
}

template<typename T>
inline auto R2Score(Turingforge::Span<T const> x, Turingforge::Span<T const> y) noexcept -> double
{
    EXPECT(y.size() == x.size());
    return R2Score(x.begin(), x.end(), y.begin());
}

template<typename T>
inline auto R2Score(Turingforge::Span<T const> x, Turingforge::Span<T const> y, Turingforge::Span<T const> w) noexcept -> double
{
    EXPECT(y.size() == x.size());
    return R2Score(x.begin(), x.end(), y.begin(), w.begin());
}
} // namespace Turingforge

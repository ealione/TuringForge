#pragma once

#include <iterator>
#include <type_traits>
#include <vstat/vstat.hpp>

#include "turing_forge/core/types.hpp"
#include "turing_forge/core/contracts.hpp"

namespace Turingforge {

    template<typename InputIt1, typename InputIt2>
    inline auto CorrelationCoefficient(InputIt1 begin1, InputIt1 end1, InputIt2 begin2) noexcept -> double
    {
        using V1 = typename std::iterator_traits<InputIt1>::value_type;
        using V2 = typename std::iterator_traits<InputIt2>::value_type;
        static_assert(std::is_arithmetic_v<V1>, "InputIt1: value_type must be arithmetic.");
        static_assert(std::is_arithmetic_v<V2>, "InputIt2: value_type must be arithmetic.");
        static_assert(std::is_same_v<V1, V2>, "The types must be the same");
        return vstat::bivariate::accumulate<V1>(begin1, end1, begin2).correlation;
    }

    template<typename InputIt1, typename InputIt2, typename InputIt3>
    inline auto CorrelationCoefficient(InputIt1 begin1, InputIt1 end1, InputIt2 begin2, InputIt3 begin3) noexcept -> double
    {
        using V1 = typename std::iterator_traits<InputIt1>::value_type;
        using V2 = typename std::iterator_traits<InputIt2>::value_type;
        static_assert(std::is_arithmetic_v<V1>, "InputIt1: value_type must be arithmetic.");
        static_assert(std::is_arithmetic_v<V2>, "InputIt2: value_type must be arithmetic.");
        static_assert(std::is_same_v<V1, V2>, "The types must be the same");
        return vstat::bivariate::accumulate<V1>(begin1, end1, begin2, begin3).correlation;
    }

    template<typename T>
    inline auto CorrelationCoefficient(Turingforge::Span<T const> x, Turingforge::Span<T const> y) -> double
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
        EXPECT(x.size() == y.size());
        EXPECT(!x.empty());
        return vstat::bivariate::accumulate<T>(x.begin(), x.end(), y.begin()).correlation;
    }

    template<typename T>
    inline auto CorrelationCoefficient(Turingforge::Span<T const> x, Turingforge::Span<T const> y, Turingforge::Span<T const> w) -> double
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
        EXPECT(x.size() == y.size());
        EXPECT(!x.empty());
        return vstat::bivariate::accumulate<T>(x.begin(), x.end(), y.begin(), w.begin()).correlation;
    }

    template<typename InputIt1, typename InputIt2>
    inline auto SquaredCorrelation(InputIt1 begin1, InputIt1 end1, InputIt2 begin2) noexcept -> double {
        auto r = CorrelationCoefficient(begin1, end1, begin2);
        return r * r;
    }

    template<typename InputIt1, typename InputIt2, typename InputIt3>
    inline auto SquaredCorrelation(InputIt1 begin1, InputIt1 end1, InputIt2 begin2, InputIt3 begin3) noexcept -> double {
        auto r = CorrelationCoefficient(begin1, end1, begin2, begin3);
        return r * r;
    }

    template<typename T>
    inline auto SquaredCorrelation(Turingforge::Span<T const> x, Turingforge::Span<T const> y) -> double {
        auto r = CorrelationCoefficient(x, y);
        return r * r;
    }

    template<typename T>
    inline auto SquaredCorrelation(Turingforge::Span<T const> x, Turingforge::Span<T const> y, Turingforge::Span<T const> w) -> double {
        auto r = CorrelationCoefficient(x, y, w);
        return r * r;
    }
    
} // namespace Turingforge
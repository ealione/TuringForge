#pragma once

#include <iterator>
#include <type_traits>
#include <vstat/vstat.hpp>
#include "turing_forge/core/types.hpp"
#include "mean_squared_error.hpp"

namespace Turingforge {

template<typename InputIt1, typename InputIt2>
inline auto RootMeanSquaredError(InputIt1 begin1, InputIt1 end1, InputIt2 begin2) noexcept -> double
{
    return std::sqrt(MeanSquaredError(begin1, end1, begin2));
}

template<typename InputIt1, typename InputIt2, typename InputIt3>
inline auto RootMeanSquaredError(InputIt1 begin1, InputIt1 end1, InputIt2 begin2, InputIt3 begin3) noexcept -> double
{
    return std::sqrt(MeanSquaredError(begin1, end1, begin2, begin3));
}

template<typename T>
inline auto RootMeanSquaredError(Turingforge::Span<T const> x, Turingforge::Span<T const> y) noexcept -> double
{
    return std::sqrt(MeanSquaredError(x, y));
}

template<typename T>
inline auto RootMeanSquaredError(Turingforge::Span<T const> x, Turingforge::Span<T const> y, Turingforge::Span<T const> w) noexcept -> double
{
    return std::sqrt(MeanSquaredError(x, y, w));
}

} // namespace Turingforge

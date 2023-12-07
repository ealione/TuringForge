#include <random>

#include "types.hpp"

namespace Turingforge
{

    // get a `Turingforge::Scalar` inside a given range
    auto RandomScalar = [](Turingforge::Scalar low, Turingforge::Scalar high)
    {
        auto randomFunc = [distribution_ = std::uniform_real_distribution<Turingforge::Scalar>(low, high),
                           random_engine_ = std::default_random_engine{std::random_device{}()}]() mutable
        {
            return distribution_(random_engine_);
        };
        return randomFunc;
    };

    template <typename R, typename T>
    auto Uniform(R &random, T a, T b) -> T
    {
        static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type.");
        using Dist = std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>;
        return Dist(a, b)(random);
    }

    template <typename R, typename InputIterator>
    auto Sample(R &random, InputIterator start, InputIterator end) -> InputIterator
    {
        auto dist = std::distance(start, end);
        if (dist <= 1)
        {
            return start;
        }
        std::advance(start, Uniform(random, decltype(dist){0}, dist - 1));
        return start;
    }

    template <typename R, typename InputIterator>
    auto Sample(R &random, InputIterator start, InputIterator end,
                std::add_pointer_t<bool(typename InputIterator::value_type const &)> condition) -> InputIterator
    {
        auto n = std::count_if(start, end, condition);

        if (n == 0)
        {
            return end; // no element satisfies the condition
        }

        auto m = Uniform(random, decltype(n){0}, n - 1);
        InputIterator it;
        for (it = start; it < end; ++it)
        {
            if (condition(*it) && 0 == m--)
            {
                break;
            }
        }

        return it;
    }

    // sample n elements and write them to the output iterator
    template <typename R, typename InputIterator, typename OutputIterator>
    auto Sample(R &random, InputIterator start, InputIterator end, OutputIterator out, size_t n) -> OutputIterator
    {
        return std::sample(start, end, out, n, random);
    }

}
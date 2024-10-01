#pragma once

#include "turing_forge/core/types.hpp"

namespace Turingforge::Backend::detail::fast_approx {
template<std::size_t P = 0>
    inline auto constexpr InvImpl(Turingforge::Scalar x) -> Turingforge::Scalar {
        static_assert(std::is_same_v<Turingforge::Scalar, float>, "this function only works in single-precision mode.");

        constexpr auto inf{ std::numeric_limits<float>::infinity() };
        constexpr auto nan = std::numeric_limits<Turingforge::Scalar>::quiet_NaN();

        if (x == -0) { return -inf; }
        if (x == +0) { return inf; }
        if (std::isinf(x)) { return 0; }
        if (std::isnan(x)) { return nan; }

        auto sx = (x < 0) ? -1.F : 1.F;
        x = sx * x;

        auto constexpr m{0x7EF127EA};
        auto xi = static_cast<int>(m - std::bit_cast<std::uint32_t>(x));
        auto xf = std::bit_cast<float>(xi);
        auto w  = x * xf;

        // Efficient Iterative Approximation Improvement in horner polynomial form.
        if constexpr (P == 1) {
            xf = xf * (2 - w);                       // perform one iteration, err = -3.36e-3 * 2^(-flr(log2(x)))
        } else if constexpr (P == 2) {
            xf = xf * (4 + w * (-6 + w * (4 - w)));  // perform two iterations, err = -1.13e-5 * 2^(-flr(log2(x)))
        } else if constexpr (P >= 3) {
            xf = xf * (8 + w * (-28 + w * (56 + w * (-70 + w *(56 + w * (-28 + w * (8 - w)))))));  // perform three iterations, err = +-6.8e-8 *  2^(-flr(log2(x)))
        }
        return xf * sx;
    }

    template<std::size_t P = 0>
    inline auto constexpr DivImpl(Turingforge::Scalar x, Turingforge::Scalar y) -> Turingforge::Scalar {
        static_assert(std::is_same_v<Turingforge::Scalar, float>, "this function only works in single-precision mode.");
        constexpr auto inf{ std::numeric_limits<Turingforge::Scalar>::infinity() };
        constexpr auto nan = std::numeric_limits<Turingforge::Scalar>::quiet_NaN();
        if (x == 0) { return y == 0 ? nan : 0.F; }
        if (std::isnan(x)) { return nan; }
        if (y == -0) { return -inf; }
        if (y == +0) { return +inf; }
        return x * InvImpl<P>(y);
    }
}  // namespace Turingforge::Backend::detail::fast_approx
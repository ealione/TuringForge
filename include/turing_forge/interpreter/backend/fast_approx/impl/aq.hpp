#pragma once

#include "turing_forge/core/types.hpp"
#include "inv.hpp"
#include "sqrt.hpp"

namespace Turingforge::Backend::detail::fast_approx {
    template<std::size_t P = 0>
    inline auto constexpr AqImpl(Turingforge::Scalar x1, Turingforge::Scalar x2) {
        auto constexpr p{9999999980506447872.F};
        return std::abs(x2) > p ? DivImpl<P>(x1, std::abs(x2)) : x1 * ISqrtImpl<P>(1 + x2*x2);
    }
} // namespace Turingforge::Backend::detail::fast_approx
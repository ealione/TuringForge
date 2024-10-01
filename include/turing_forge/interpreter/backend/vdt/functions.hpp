#pragma once

#include <vdt/vdt.h>

#include "turing_forge/interpreter/backend/backend.hpp"
#include "turing_forge/core/function.hpp"

namespace Turingforge::Backend {

    namespace detail::vdt {
        // we need wrappers due to VDT calling conventions
        inline auto Exp(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            auto constexpr nan{ std::numeric_limits<Turingforge::Scalar>::quiet_NaN() };
            auto constexpr inf{ std::numeric_limits<Turingforge::Scalar>::infinity() };

            if (std::isnan(x)) { return nan; }
            if (x == 0) { return 1.F; }
            if (x == -inf) { return 0; }
            if (x == +inf) { return inf; }

            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_expf(x); }
            else { return ::vdt::fast_exp(x); }
        }

        inline auto Log(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            auto constexpr nan{ std::numeric_limits<Turingforge::Scalar>::quiet_NaN() };
            auto constexpr inf{ std::numeric_limits<Turingforge::Scalar>::infinity() };

            if (std::isnan(x)) { return nan; }
            if (x < 0) { return nan; }
            if (x == 0) { return -inf; }
            if (x == 1) { return 0.F; }
            if (x == inf) { return inf; }

            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_logf(x); }
            else { return ::vdt::fast_log(x); }
        }

        inline auto Logabs(Turingforge::Scalar x) -> Turingforge::Scalar {
            return Log(std::abs(x));
        }

        inline auto Log1p(Turingforge::Scalar x) -> Turingforge::Scalar {
            return Log(1 + x);
        }

        inline auto Inv(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_invf(x); }
            else { return ::vdt::fast_inv(x); }
        }

        inline auto ISqrt(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            auto constexpr nan{ std::numeric_limits<Turingforge::Scalar>::quiet_NaN() };
            auto constexpr inf{ std::numeric_limits<Turingforge::Scalar>::infinity() };

            if (std::isnan(x)) { return nan; }
            if (x < 0) { return nan; }
            if (x == -0) { return -inf; }
            if (x == +0) { return +inf; }

            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_isqrtf(x); }
            else { return ::vdt::fast_isqrt(x); }
        }

        inline auto Sqrt(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            constexpr auto nan = std::numeric_limits<Turingforge::Scalar>::quiet_NaN();
            if (std::isnan(x)) { return nan; }
            if (x < 0) { return nan; }
            if (x == 0) { return 0; }
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return x * ::vdt::fast_isqrtf(x); }
            else { return x * ::vdt::fast_isqrt(x); }
        }

        inline auto Sqrtabs(Turingforge::Scalar x) -> Turingforge::Scalar {
            return Sqrt(std::abs(x));
        }

        inline auto Cbrt(Turingforge::Scalar x) -> Turingforge::Scalar {
            return std::cbrt(x);
        }

        inline auto Floor(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            return static_cast<Turingforge::Scalar>(::vdt::details::fpfloor(x));
        }

        inline auto Div(Turingforge::Scalar x, Turingforge::Scalar y) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            return x * Inv(y);
        }

        inline auto Aq(Turingforge::Scalar x, Turingforge::Scalar y) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            return x * ISqrt(Turingforge::Scalar{1} + y * y);
        }

        inline auto Pow(Turingforge::Scalar x, Turingforge::Scalar y) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            auto constexpr nan{ std::numeric_limits<Turingforge::Scalar>::quiet_NaN() };
            auto constexpr inf{ std::numeric_limits<Turingforge::Scalar>::infinity() };

            if (std::isnan(x)) { return nan; }
            if (std::isnan(y)) { return nan; }
            if (x == 0) { return y < 0 ? inf : x; }
            if (x < 0) { return nan; }
            if (y == 0) { return 1.F; }
            if (y == -inf) { return 0; }
            if (y == +inf) { return inf; }

            return Exp(y * Log(x));
        }

        inline auto Acos(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_acosf(x); }
            else { return ::vdt::fast_acos(x); }
        }

        inline auto Asin(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_asinf(x); }
            else { return ::vdt::fast_asin(x); }
        }

        inline auto Atan(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_atanf(x); }
            else { return ::vdt::fast_atan(x); }
        }

        inline auto Cos(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if (!std::isfinite(x)) { return std::numeric_limits<Turingforge::Scalar>::quiet_NaN(); }
            if (x == 0) { return 1.F; }
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_cosf(x); }
            else { return ::vdt::fast_cos(x); }
        }

        inline auto Sin(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if (!std::isfinite(x)) { return std::numeric_limits<Turingforge::Scalar>::quiet_NaN(); }
            if (x == 0) { return x; }
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_sinf(x); }
            else { return ::vdt::fast_sin(x); }
        }

        inline auto Tan(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_tanf(x); }
            else { return ::vdt::fast_tan(x); }
        }

        inline auto Sinh(Turingforge::Scalar x) -> Turingforge::Scalar {
            auto const e = Exp(x);
            return Div(e * e - Turingforge::Scalar{1}, e + e);
        }

        inline auto Cosh(Turingforge::Scalar x) -> Turingforge::Scalar {
            auto const e = Exp(x);
            return Div(e * e + Turingforge::Scalar{1}, e + e);
        }

        inline auto Tanh(Turingforge::Scalar x) -> Turingforge::Scalar {
            static_assert(std::is_arithmetic_v<decltype(x)>);
            constexpr auto nan { std::numeric_limits<Turingforge::Scalar>::quiet_NaN() };
            if (std::isnan(x)) { return nan; }
            if (x == 0) { return 0; }

            if constexpr (std::is_same_v<Turingforge::Scalar, float>) { return ::vdt::fast_tanhf(x); }
            else { return ::vdt::fast_tanh(x); }
        }
    } // namespace detail::vdt

    // utility
    template<typename T, std::size_t S>
    auto Fill(T* res, T value) {
        std::ranges::fill_n(res, S, value);
    }

    template<typename T, std::size_t S>
    auto Fill(T* res, int n, T value) {
        std::ranges::fill_n(res, n, value);
    }

    // unary functions
    template<typename T, std::size_t S>
    auto Add(T* res, auto const*... args) {
        for (auto i = 0UL; i < S; ++i) {
            res[i] = (args[i] + ...);
        }
    }

    template<typename T, std::size_t S>
    auto Mul(T* res, auto const*... args) {
        for (auto i = 0UL; i < S; ++i) {
            res[i] = (args[i] * ...);
        }
    }

    template<typename T, std::size_t S>
    auto Sub(T* res, auto const* first, auto const*... rest) {
        for (auto i = 0UL; i < S; ++i) {
            if constexpr (sizeof...(rest) == 0) {
                res[i] = -first[i];
            } else {
                res[i] = first[i] - (rest[i] + ...);
            }
        }
    }

    template<typename T, std::size_t S>
    auto Div(T* res, auto const* first, auto const*... rest) {
        for (auto i = 0UL; i < S; ++i) {
            if constexpr (sizeof...(rest) == 0) {
                res[i] = detail::vdt::Inv(first[i]);
            } else {
                res[i] = detail::vdt::Div(first[i], (rest[i] * ...));
            }
        }
    }

    template<typename T, std::size_t S>
    auto Min(T* res, auto const*... args) {
        for (auto i = 0UL; i < S; ++i) {
            res[i] = std::min({args[i]...});
        }
    }

    template<typename T, std::size_t S>
    auto Max(T* res, auto const*... args) {
        for (auto i = 0UL; i < S; ++i) {
            res[i] = std::max({args[i]...});
        }
    }

    // binary functions
    template<typename T, std::size_t S>
    auto Aq(T* res, T const* a, T const* b) {
        std::transform(a, a+S, b, res, detail::vdt::Aq);
    }

    template<typename T, std::size_t S>
    auto Pow(T* res, T const* a, T const* b) {
        std::transform(a, a+S, b, res, detail::vdt::Pow);
    }

    // unary functions
    template<typename T, std::size_t S>
    auto Cpy(T* res, T const* arg) {
        std::ranges::copy_n(arg, S, res);
    }

    template<typename T, std::size_t S>
    auto Neg(T* res, T const* arg) {
        std::transform(arg, arg+S, res, std::negate{});
    }

    template<typename T, std::size_t S>
    auto Inv(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Inv);
    }

    template<typename T, std::size_t S>
    auto Abs(T* res, T const* arg) {
        std::transform(arg, arg+S, res, [](auto x) { return std::abs(x); });
    }

    template<typename T, std::size_t S>
    auto Square(T* res, T const* arg) {
        std::transform(arg, arg+S, res, [](auto x) { return x * x; });
    }

    template<typename T, std::size_t S>
    auto Ceil(T* res, T const* arg) {
        std::transform(arg, arg+S, res, [](auto x) { return std::ceil(x); });
    }

    template<typename T, std::size_t S>
    auto Floor(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Floor);
    }

    template<typename T, std::size_t S>
    auto Exp(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Exp);
    }

    template<typename T, std::size_t S>
    auto Log(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Log);
    }

    template<typename T, std::size_t S>
    auto Log1p(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Log1p);
    }

    template<typename T, std::size_t S>
    auto Logabs(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Logabs);
    }

    template<typename T, std::size_t S>
    auto Sin(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Sin);
    }

    template<typename T, std::size_t S>
    auto Cos(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Cos);
    }

    template<typename T, std::size_t S>
    auto Tan(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Tan);
    }

    template<typename T, std::size_t S>
    auto Asin(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Asin);
    }

    template<typename T, std::size_t S>
    auto Acos(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Acos);
    }

    template<typename T, std::size_t S>
    auto Atan(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Atan);
    }

    template<typename T, std::size_t S>
    auto Sinh(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Sinh);
    }

    template<typename T, std::size_t S>
    auto Cosh(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Cosh);
    }

    template<typename T, std::size_t S>
    auto Tanh(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Tanh);
    }

    template<typename T, std::size_t S>
    auto Sqrt(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Sqrt);
    }

    template<typename T, std::size_t S>
    auto Sqrtabs(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Sqrtabs);
    }

    template<typename T, std::size_t S>
    auto Cbrt(T* res, T const* arg) {
        std::transform(arg, arg+S, res, detail::vdt::Cbrt);
    }
} // namespace Turingforge::Backend
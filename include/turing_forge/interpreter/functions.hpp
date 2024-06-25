#pragma once

#include "turing_forge/core/function.hpp"

#if defined(TURINGFORGE_MATH_EIGEN)
#include "turing_forge/interpreter/backend/eigen.hpp"
#elif defined(TURINGFORGE_MATH_EVE)
#include "turing_forge/interpreter/backend/eve.hpp"
#elif defined(TURINGFORGE_MATH_ARMA)
#include "turing_forge/interpreter/backend/arma.hpp"
#elif defined(TURINGFORGE_MATH_BLAZE)
#include "turing_forge/interpreter/backend/blaze.hpp"
#elif defined(TURINGFORGE_MATH_FASTOR)
#include "turing_forge/interpreter/backend/fastor.hpp"
#elif defined(TURINGFORGE_MATH_STL)
#include "turing_forge/interpreter/backend/plain.hpp"
#elif defined(TURINGFORGE_MATH_VDT)
#include "turing_forge/interpreter/backend/vdt.hpp"
#elif defined(TURINGFORGE_MATH_XTENSOR)
#include "turing_forge/interpreter/backend/xtensor.hpp"
#elif defined(TURINGFORGE_MATH_FAST_V1)
#include "turing_forge/interpreter/backend/fast_v1.hpp"
#elif defined(TURINGFORGE_MATH_FAST_V2)
#include "turing_forge/interpreter/backend/fast_v2.hpp"
#elif defined(TURINGFORGE_MATH_FAST_V3)
#include "turing_forge/interpreter/backend/fast_v3.hpp"
#else
#include "turing_forge/interpreter/backend/plain.hpp"
#endif

namespace Turingforge {
    // utility
    template<typename T, std::size_t S>
    auto Fill(Backend::View<T, S> view, int idx, T value) {
        auto* p = view.data_handle() + idx * S;
        std::fill_n(p, S, value);
    };

    // detect missing specializations
    template<typename T, Turingforge::FunctionType N = Turingforge::FunctionTypes::NoType, bool C = false, std::size_t S = Backend::BatchSize<T>>
    struct Func {
        auto operator()(Backend::View<T, S> /*unused*/, std::integral auto /*unused*/, std::integral auto... /*unused*/) {
            throw std::runtime_error(fmt::format("backend error: missing specialization for function: {}\n", Turingforge::Function{N}.Name()));
        }
    };

    // n-ary operations
    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Add, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto... args) {
            auto* h = view.data_handle();
            if constexpr (C) {
                Backend::Add<T, S>(h + result * S, h + result * S, (h + args * S)...);
            } else {
                Backend::Add<T, S>(h + result * S, (h + args * S)...);
            }
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Mul, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto... args) {
            auto* h = view.data_handle();
            if constexpr (C) {
                Backend::Mul<T, S>(h + result * S, h + result * S, (h + args * S)...);
            } else {
                Backend::Mul<T, S>(h + result * S, (h + args * S)...);
            }
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Sub, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto first, std::integral auto... args) {
            auto* h = view.data_handle();

            if constexpr (C) {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Sub<T, S>(h + result * S, h + result * S, h + first * S);
                } else {
                    Backend::Sub<T, S>(h + result * S, h + result * S, h + first * S, (h + args * S)...);
                }
            } else {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Neg<T, S>(h + result * S, h + first * S);
                } else {
                    Backend::Sub<T, S>(h + result * S, h + first * S, (h + args * S)...);
                }
            }
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Div, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto first, std::integral auto... args) {
            auto* h = view.data_handle();

            if constexpr (C) {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Div<T, S>(h + result * S, h + result * S, h + first * S);
                } else {
                    Backend::Div<T, S>(h + result * S, h + result * S, h + first * S, (h + args * S)...);
                }
            } else {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Inv<T, S>(h + result * S, h + first * S);
                } else {
                    Backend::Div<T, S>(h + result * S, h + first * S, (h + args * S)...);
                }
            }
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Fmin, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto first, std::integral auto... args) {
            auto* h = view.data_handle();

            if constexpr (C) {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Min<T, S>(h + result * S, h + result * S, h + first * S);
                } else {
                    Backend::Min<T, S>(h + result * S, h + result * S, h + first * S, (h + args * S)...);
                }
            } else {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Cpy<T, S>(h + result * S, h + first * S);
                } else {
                    Backend::Min<T, S>(h + result * S, h + first * S, (h + args * S)...);
                }
            }
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Fmax, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto first, std::integral auto... args) {
            auto* h = view.data_handle();

            if constexpr (C) {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Max<T, S>(h + result * S, h + result * S, h + first * S);
                } else {
                    Backend::Max<T, S>(h + result * S, h + result * S, h + first * S, (h + args * S)...);
                }
            } else {
                if constexpr (sizeof...(args) == 0) {
                    Backend::Cpy<T, S>(h + result * S, h + first * S);
                } else {
                    Backend::Max<T, S>(h + result * S, h + first * S, (h + args * S)...);
                }
            }
        }
    };

    // binary operations
    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Aq, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i, std::integral auto j) {
            auto* h = view.data_handle();
            Backend::Aq<T, S>(h + result * S, h + i * S, h + j * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Pow, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i, std::integral auto j) {
            auto* h = view.data_handle();
            Backend::Pow<T, S>(h + result * S, h + i * S, h + j * S);
        }
    };

    // unary operations
    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Abs, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Abs<T, S>(h + result * S, h + i * S);
        }
    };

    // unary operations
    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Square, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Square<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Exp, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Exp<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Log, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Log<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Logabs, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Logabs<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Log1p, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Log1p<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Sqrt, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Sqrt<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Sqrtabs, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Sqrtabs<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Cbrt, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Cbrt<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Ceil, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Ceil<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Floor, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Floor<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Sin, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Sin<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Cos, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Cos<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Tan, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Tan<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Asin, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Asin<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Acos, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Acos<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Atan, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Atan<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Sinh, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Sinh<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Cosh, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Cosh<T, S>(h + result * S, h + i * S);
        }
    };

    template<typename T, bool C, std::size_t S>
    struct Func<T, Turingforge::FunctionType::Tanh, C, S> {
        auto operator()(Backend::View<T, S> view, std::integral auto result, std::integral auto i) {
            auto* h = view.data_handle();
            Backend::Tanh<T, S>(h + result * S, h + i * S);
        }
    };
} // namespace Turingforge
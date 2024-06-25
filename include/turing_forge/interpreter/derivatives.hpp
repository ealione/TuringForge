#pragma once

#include <functional>

#include "functions.hpp"
#include "dual.hpp"

namespace Turingforge {
    // default function to catch any missing template specializations
    template<typename T, Turingforge::FunctionType N  = Turingforge::FunctionTypes::NoType, std::size_t S = Backend::BatchSize<T>>
    struct Diff {
        auto operator()(std::vector<Turingforge::Function> const&, Backend::View<T const, S>, Backend::View<T>, std::integral auto, std::integral auto) {
            throw std::runtime_error(fmt::format("backend error: missing specialization for derivative: {}\n", Turingforge::Function{N}.Name()));
        }
    };

    // n-ary functions
    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Add, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Add<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Sub, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Sub<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Mul, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Mul<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Div, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Div<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Fmin, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Min<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Fmax, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Max<T, S>(nodes, primal, trace, i, j);
        }
    };

    // binary functions
    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Aq, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Aq<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Pow, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Pow<T, S>(nodes, primal, trace, i, j);
        }
    };

    // unary functions
    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Abs, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Abs<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Acos, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Acos<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Asin, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Asin<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Atan, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Atan<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Cbrt, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Cbrt<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Ceil, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Ceil<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Cos, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Cos<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Cosh, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Cosh<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Exp, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Exp<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Floor, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Floor<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Log, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Log<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Logabs, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Logabs<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Log1p, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Log1p<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Sin, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Sin<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Sinh, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Sinh<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Sqrt, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Sqrt<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Sqrtabs, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Sqrtabs<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Square, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Square<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Tan, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Tan<T, S>(nodes, primal, trace, i, j);
        }
    };

    template<typename T, std::size_t S>
    struct Diff<T, Turingforge::FunctionType::Tanh, S> {
        auto operator()(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
            Backend::Tanh<T, S>(nodes, primal, trace, i, j);
        }
    };
} // namespace Turingforge
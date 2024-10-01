#pragma once

#include "turing_forge/core/function.hpp"
#include "functions.hpp"

namespace Turingforge::Backend {
namespace detail {
    template<typename T>
    inline auto IsNaN(T value) { return std::isnan(value); }

    template<typename Compare>
    struct FComp {
        auto operator()(auto x, auto y) const {
            using T = std::common_type_t<decltype(x), decltype(y)>;
            if ((IsNaN(x) && IsNaN(y)) || (x == y)) {
                return std::numeric_limits<T>::quiet_NaN();
            }
            if (IsNaN(x)) { return T{0}; }
            if (IsNaN(y)) { return T{1}; }
            return static_cast<T>(Compare{}(T{x}, T{y}));
        }
    };
} // namespace detail

    template<typename T, std::size_t S>
    auto Add(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> /*primal*/, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j).fill(T{1});
    }

    template<typename T, std::size_t S>
    auto Mul(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        Col(trace, j) = Col(primal, i) / Col(primal, j);
    }

    template<typename T, std::size_t S>
    auto Sub(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> /*primal*/, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        auto v = (nodes[i].Arity == 1 || j < i-1) ? T{-1} : T{+1};
        Col(trace, j).fill(v);
    }

    template<typename T, std::size_t S>
    auto Div(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        auto const& n = nodes[i];
        if (n.Arity == 1) {
            Col(trace, j) = -T{1} / (arma::square(Col(primal, j)));
        } else {
            Col(trace, j) = (j == i-1 ? T{1} : T{-1}) * Col(primal, i) / Col(primal, j);
        }
    }

    template<typename T, std::size_t S>
    auto Aq(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        if (j == i-1) {
            Col(trace, j) = Col(primal, i) / Col(primal, j);
        } else {
            Col(trace, j) = -Col(primal, j) % arma::pow(Col(primal, i), T{3}) / arma::square(Col(primal, i-1));
        }
    }

    template<typename T, std::size_t S>
    auto Pow(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        if (j == i-1) {
            auto const k = j - (nodes[j].Length + 1);
            Col(trace, j) = Col(primal, i) % Col(primal, k) / Col(primal, j);
        } else {
            auto const k = i-1;
            Col(trace, j) = Col(primal, i) % arma::log(Col(primal, k));
        }
    }

    template<typename T, std::size_t S>
    auto Min(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        auto k = j == i - 1 ? (j - nodes[j].Length - 1) : i - 1;
        auto const* jp = Ptr(primal, j);
        auto const* kp = Ptr(primal, k);
        std::transform(jp, jp+S, kp, Ptr(trace, j), detail::FComp<std::less<>>{});
    }

    template<typename T, std::size_t S>
    auto Max(std::vector<Turingforge::Function> const& nodes, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        auto k = j == i - 1 ? (j - nodes[j].Length - 1) : i - 1;
        auto const* jp = Ptr(primal, j);
        auto const* kp = Ptr(primal, k);
        std::transform(jp, jp+S, kp, Ptr(trace, j), detail::FComp<std::greater<>>{});
    }

    template<typename T, std::size_t S>
    auto Square(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{2} * Col(primal, j);
    }

    template<typename T, std::size_t S>
    auto Abs(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::sign(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Ceil(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::ceil(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Floor(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::floor(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Exp(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        Col(trace, j) = Col(primal, i);
    }

    template<typename T, std::size_t S>
    auto Log(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{1} / (Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Log1p(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{1} / (T{1} + Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Logabs(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::sign(Col(primal, j)) / arma::abs(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Sin(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::cos(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Cos(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = -arma::sin(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Tan(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{1} + arma::square(arma::tan(Col(primal, j)));
    }

    template<typename T, std::size_t S>
    auto Sinh(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::cosh(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Cosh(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = arma::sinh(Col(primal, j));
    }

    template<typename T, std::size_t S>
    auto Tanh(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{1} - arma::square(arma::tanh(Col(primal, j)));
    }

    template<typename T, std::size_t S>
    auto Asin(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{1} / (arma::sqrt(T{1} - arma::square(Col(primal, j))));
    }

    template<typename T, std::size_t S>
    auto Acos(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = -T{1} / (arma::sqrt(((T{1} - arma::square(Col(primal, j))))));
    }

    template<typename T, std::size_t S>
    auto Atan(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto /*i*/, std::integral auto j) {
        Col(trace, j) = T{1} / (T{1} + arma::square(Col(primal, j)));
    }

    template<typename T, std::size_t S>
    auto Sqrt(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        Col(trace, j) = T{1} / (T{2} * Col(primal, i));
    }

    template<typename T, std::size_t S>
    auto Sqrtabs(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        Col(trace, j) = arma::sign(Col(primal, j)) / (T{2} * Col(primal, i));
    }

    template<typename T, std::size_t S>
    auto Cbrt(std::vector<Turingforge::Function> const& /*nodes*/, Backend::View<T const, S> primal, Backend::View<T> trace, std::integral auto i, std::integral auto j) {
        Col(trace, j) = T{1} / (T{3} * arma::square(Col(primal, i)));
    }
}  // namespace Turingforge::Backend
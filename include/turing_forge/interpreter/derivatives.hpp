#pragma once

#include <functional>

#include "functions.hpp"
#include "dual.hpp"

namespace Turingforge {
    namespace detail {
        template<typename T>
        inline auto IsNaN(T value) { return std::isnan(value); }

        template<>
        inline auto IsNaN<Turingforge::Dual>(Turingforge::Dual value) { return ceres::isnan(value); } // NOLINT

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

    template<Turingforge::FunctionType N = Turingforge::FunctionType::Add>
    struct Diff {
        auto operator()(auto const& /*nodes*/, auto const& /*primal*/, auto& trace, int /*parent*/, int j) {
            trace.col(j).setConstant(scalar_t<decltype(trace)>{1});
        }
    };

    template<Turingforge::FunctionType N = Turingforge::FunctionType::Add>
    struct Diff2 {
        auto operator()(auto const& /*nodes*/, auto const& /*primal*/, auto& trace, int /*parent*/, int j) {
            trace.col(j).setConstant(scalar_t<decltype(trace)>{0});
        }
    };

    template<>
    struct Diff<Turingforge::FunctionType::Mul> {
        auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, int i, int j) {
            trace.col(j) = primal.col(i) / primal.col(j);
        }
    };

    template<>
    struct Diff2<Turingforge::FunctionType::Mul> {
        auto operator()(auto const& /*nodes*/, auto const& /*primal*/, auto& trace, int /*parent*/, int j) {
            trace.col(j).setConstant(scalar_t<decltype(trace)>{1});
        }
    };

    template<>
    struct Diff<Turingforge::FunctionType::Sub> {
        auto operator()(auto const& nodes, auto const& /*primal*/, auto& trace, int i, int j) {
            auto v = (nodes[i].Arity == 1 || j < i-1) ? -1 : +1;
            trace.col(j).setConstant(scalar_t<decltype(trace)>(v));
        }
    };

    template<>
    struct Diff2<Turingforge::FunctionType::Sub> {
        auto operator()(auto const& /*nodes*/, auto const& /*primal*/, auto& trace, int /*parent*/, int j) {
            trace.col(j).setConstant(scalar_t<decltype(trace)>{0});
        }
    };

    template<>
    struct Diff<Turingforge::FunctionType::Div> {
        auto operator()(auto const& nodes, auto const& primal, auto& trace, int i, int j) {
            auto const& n = nodes[i];

            if (n.Arity == 1) {
                trace.col(j) = -primal.col(j).square().inverse();
            } else {
                auto v = scalar_t<decltype(trace)>{1.0};
                trace.col(j) = (j == i-1 ? +v : -v) * primal.col(i) / primal.col(j);
            }
        }
    };

    template<>
    struct Diff2<Turingforge::FunctionType::Div> {
        auto operator()(auto const& nodes, auto const& primal, auto& trace, int i, int j) {
            auto const& n = nodes[i];

            if (n.Arity == 1) {
                trace.col(j) = 2 * primal.col(j).pow(scalar_t<decltype(trace)>{3.0}).inverse();
            } else {
                auto v = scalar_t<decltype(trace)>{1.0};
                trace.col(j) = (j == i-1 ? +v : -v) * primal.col(i) / primal.col(j);
            }
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Aq> {
        auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, int i, int j) {
            if (j == i-1) {
                // first arg
                trace.col(j) = primal.col(i) / primal.col(j);
            } else {
                // second arg
                trace.col(j) = -primal.col(j) * primal.col(i).pow(scalar_t<decltype(trace)>(3)) / primal.col(i-1).square();
            }
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Pow> {
        auto operator()(auto const& nodes, auto const& primal, auto& trace, auto i, auto j)
        {
            if (j == i-1) {
                // first arg
                auto const k = j - (nodes[j].Length + 1);
                trace.col(j) = primal.col(i) * primal.col(k) / primal.col(j);
            } else {
                // second arg
                auto const k = i - 1;
                trace.col(j) = primal.col(i) * primal.col(k).log();
            }
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Fmin> {
        auto operator()(auto const& nodes, auto const& primal, auto& trace, auto i, auto j)
        {
            auto k = j == i-1 ? (j - nodes[j].Length - 1) : i-1;
            trace.col(j) = primal.col(j).binaryExpr(primal.col(k), detail::FComp<std::less<>>{});
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Fmax> {
        auto operator()(auto const& nodes, auto const& primal, auto& trace, auto i, auto j)
        {
            auto k = j == i-1 ? (j - nodes[j].Length - 1) : i-1;
            trace.col(j) = primal.col(j).binaryExpr(primal.col(k), detail::FComp<std::greater<>>{});
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Square> {
        auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = scalar_t<decltype(trace)>{2} * primal.col(j);
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Abs> {
        auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = primal.col(j).sign();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Exp> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto i, auto j)
        {
            trace.col(j) = primal.col(i);
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Log> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = primal.col(j).inverse();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Logabs> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = primal.col(j).sign() / primal.col(j).abs();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Log1p> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = (primal.col(j) + scalar_t<decltype(trace)>{1}).inverse();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Sin> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = primal.col(j).cos();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Sinh> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = primal.col(j).cosh();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Cos> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = -primal.col(j).sin();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Cosh> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = primal.col(j).sinh();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Tan> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = scalar_t<decltype(trace)>{1} + primal.col(j).tan().square();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Tanh> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = scalar_t<decltype(trace)>{1} - primal.col(j).tanh().square();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Asin> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = (scalar_t<decltype(trace)>{1} - primal.col(j).square()).sqrt().inverse();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Acos> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = -(scalar_t<decltype(trace)>{1} - primal.col(j).square()).sqrt().inverse();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Atan> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto /*i*/, auto j)
        {
            trace.col(j) = (scalar_t<decltype(trace)>{1} + primal.col(j).square()).inverse();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Sqrt> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto i, auto j)
        {
            trace.col(j) = (scalar_t<decltype(trace)>{2} * primal.col(i)).inverse();
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Sqrtabs> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto i, auto j)
        {
            trace.col(j) = sign(primal.col(j)) / (scalar_t<decltype(trace)>{2} * primal.col(i));
        }
    };

    template <>
    struct Diff<Turingforge::FunctionType::Cbrt> {
        inline auto operator()(auto const& /*nodes*/, auto const& primal, auto& trace, auto i, auto j)
        {
            trace.col(j) = (scalar_t<decltype(trace)>{3} * (primal.col(i)).square()).inverse();
        }
    };
} // namespace Turingforge
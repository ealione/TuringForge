#pragma once

#include <algorithm>
#include <optional>
#include <type_traits>
#include <utility>

#include "turing_forge/core/dataset.hpp"
#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/types.hpp"
#include "dispatch_table.hpp"

namespace Turingforge {

    enum class LikelihoodType : int { Gaussian, Poisson };

    template<typename T>
    struct InterpreterBase {
        // evaluate model output
        virtual auto Evaluate(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range, Turingforge::Span<T> result) const -> void = 0;
        virtual auto Evaluate(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const -> std::vector<T> = 0;

        // evaluate model jacobian in reverse mode
        virtual auto JacRev(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range, Turingforge::Span<T> jacobian) const -> void = 0;
        virtual auto JacRev(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const -> Eigen::Array<T, -1, -1> = 0;

        // evaluate model jacobian in forward mode
        virtual auto JacFwd(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range, Turingforge::Span<T> jacobian) const -> void = 0;
        virtual auto JacFwd(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const -> Eigen::Array<T, -1, -1> = 0;

        // getters
        [[nodiscard]] virtual auto GetTree() const -> Turingforge::Individual const& = 0;
        [[nodiscard]] virtual auto GetDataset() const -> Turingforge::Dataset const& = 0;

        virtual ~InterpreterBase() = default;
    };

    template<typename T = Turingforge::Scalar, typename DTable = DefaultDispatch>
    requires DTable::template SupportsType<T>
    struct Interpreter : public InterpreterBase<T> {
        using DispatchTable = DTable;
        static constexpr auto BatchSize = DTable::template BatchSize<T>;

        Interpreter(DTable const& dtable, Turingforge::Dataset const& dataset, Turingforge::Individual const& individual)
                : dtable_(dtable)
                , dataset_(dataset)
                , individual_(individual) { }

        auto Primal() const { return primal_; }
        auto Trace() const { return trace_; }

        inline auto Evaluate(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range, Turingforge::Span<T> result) const -> void final {
            InitContext(coeff, range);

            auto res = primal_.col(primal_.cols()-1);
            auto const len{ static_cast<int64_t>(range.Size()) };

            constexpr int64_t S{ BatchSize };
            for (auto row = 0L; row < len; row += S) {
                ForwardPass(range, row, /*trace=*/false);

                if (std::ssize(result) == len) {
                    auto rem = std::min(S, len - row);
                    Eigen::Map<Eigen::Array<T, -1, 1>>(result.data(), result.size()).segment(row, rem) = res.head(rem);
                }
            }
        }

        inline auto Evaluate(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const -> std::vector<T> final {
            std::vector<T> res(range.Size());
            Evaluate(coeff, range, {res.data(), res.size()});
            return res;
        }

        inline auto JacRev(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range, Turingforge::Span<T> jacobian) const -> void final {
            InitContext(coeff, range);
            auto const len{ static_cast<int64_t>(range.Size()) };
            auto const& polynomials = individual_.get().GetCoefficients();
            auto const nn { std::ssize(polynomials) };

            constexpr int64_t S{ BatchSize };
            trace_ = Eigen::Array<T, S, -1>::Zero(S, nn);
            trace_.col(nn-1).setConstant(T{1});

            Eigen::Map<Eigen::Array<T, -1, -1>> jac(jacobian.data(), len, coeff.size());

            for (auto row = 0L; row < len; row += S) {
                ForwardPass(range, row, /*trace=*/true);
                ReverseTrace(range, row, jac);
            }
        }

        inline auto JacRev(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const -> Eigen::Array<T, -1, -1> final {
            auto const nr{ static_cast<int64_t>(range.Size()) };
            Eigen::Array<T, -1, -1> jacobian(nr, coeff.size());
            JacRev(coeff, range, { jacobian.data(), static_cast<size_t>(jacobian.size()) });
            return jacobian;
        }

        auto JacFwd(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range, Turingforge::Span<T> jacobian) const -> void final {
            InitContext(coeff, range);
            auto const len{ static_cast<int>(range.Size()) };
            auto const& polynomials = individual_.get().GetCoefficients();
            auto const nn   { std::ssize(polynomials) };

            constexpr int64_t S{ BatchSize };
            trace_ = DTable::template Array<T>::Zero(S, nn);
            trace_.col(nn-1).setConstant(T{1});

            Eigen::Map<Eigen::Array<T, -1, -1>> jac(jacobian.data(), len, coeff.size());

            for (int row = 0; row < len; row += S) {
                ForwardPass(range, row, /*trace=*/true);
                ForwardTrace(range, row, jac);
            }
        }

        inline auto JacFwd(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const -> Eigen::Array<T, -1, -1> final {
            auto const nr{ static_cast<int64_t>(range.Size()) };
            Eigen::Array<T, -1, -1> jacobian(nr, coeff.size());
            JacFwd(coeff, range, { jacobian.data(), static_cast<size_t>(jacobian.size()) });
            return jacobian;
        }

        [[nodiscard]] auto GetTree() const -> Turingforge::Individual const& final { return individual_.get(); }
        [[nodiscard]] auto GetDataset() const -> Turingforge::Dataset const& final { return dataset_.get(); }

        auto GetDispatchTable() const { return dtable_.get(); }

        static inline auto Evaluate(Turingforge::Individual const& individual, Turingforge::Dataset const& dataset, Turingforge::Range const range) {
            auto coeff = individual.GetCoefficients();
            DTable dt;
            return Interpreter{dt, dataset, individual}.Evaluate(coeff, range);
        }

        static inline auto Evaluate(Turingforge::Individual const& individual, Turingforge::Dataset const& dataset, Turingforge::Range const range, Turingforge::Span<T const> coeff) {
            DTable dt;
            return Interpreter{dt, dataset, individual}.Evaluate(coeff, range);
        }

    private:
        // private members
        using Data = std::tuple<T,
                Eigen::Map<Eigen::Array<Turingforge::Scalar, -1, 1> const>,
                std::optional<Dispatch::Callable<typename DTable::template Array<T>> const>,
        std::optional<Dispatch::CallableDiff<typename DTable::template Array<T>> const> >;

        std::reference_wrapper<DTable const> dtable_;
        std::reference_wrapper<Turingforge::Dataset const> dataset_;
        std::reference_wrapper<Turingforge::Individual const> individual_;

        // mutable internal state (used by all the forward/reverse passes)
        mutable std::vector<Data> context_;
        mutable typename DTable::template Array<T> primal_;
        mutable typename DTable::template Array<T> trace_;

        // private methods
        inline auto ForwardPass(Turingforge::Range range, int row, bool trace = false) const -> void {
            auto const start { static_cast<int64_t>(range.Start()) };
            auto const len   { static_cast<int64_t>(range.Size()) };
            auto const& polynomials = individual_.get().GetCoefficients();
            auto const nn = std::ssize(polynomials);
            constexpr int64_t S{ BatchSize };

            auto rem = std::min(S, len - row);
            Turingforge::Range rg(start + row, start + row + rem);

            // forward pass - compute primal and trace
            for (auto i = 0L; i < nn; ++i) {
                auto const& [ p, v, f, df ] = context_[i];
                std::invoke(*f, polynomials, primal_, i, rg);
//                if (polynomials[i].IsVariable()) {
//                    primal_.col(i).head(rem) = p * v.segment(row, rem).template cast<T>();
//                } else if (f) {
//                    std::invoke(*f, polynomials, primal_, i, rg);

                    // first compute the partials
//                    if (trace && df) {
//                        for (auto j : Individual::GetFunctions(i)) {
//                            std::invoke(*df, polynomials, primal_, trace_, i, j);
//                        }
//                    }

                    // apply weight after partials are computed
                    if (p != T{1}) { primal_.col(i).head(rem) *= p; }
//                }
            }
        }

        inline auto ForwardTrace(Turingforge::Range range, int row, Eigen::Ref<Eigen::Array<T, -1, -1>> jac) const -> void {
            auto const len   { static_cast<int64_t>(range.Size()) };
            auto const& polynomials{ individual_.get().GetCoefficients() };
            auto const nn { std::ssize(polynomials) };
            constexpr int64_t S{ BatchSize };
            auto const rem   { std::min(S, len - row) };

            typename DTable::template Array<T> dot(S, nn);
            std::vector<int64_t> cidx(jac.cols());

//            for (auto i = 0L, j = 0L; i < nn; ++i) {
//                if (polynomials[i].Optimize) { cidx[j++] = i; }
//            }

            auto k{0};
            for (auto c : cidx) {
                dot.topRows(rem).setConstant(T{0});
                dot.col(c).head(rem).setConstant(T{1});

//                for (auto i = 0; i < nn; ++i) {
//                    for (auto x : Individual::GetFunctions(i))  {
//                        auto j{ static_cast<int64_t>(x) };
//                        dot.col(i).head(rem) += dot.col(j).head(rem) * trace_.col(j).head(rem) * std::get<0>(context_[i]);
//                    }
//                }

                jac.col(k++).segment(row, rem) = dot.col(nn-1).head(rem) * primal_.col(c).head(rem) / std::get<0>(context_[c]);
            }
        }

        auto ReverseTrace(Turingforge::Range range, int row, Eigen::Ref<Eigen::Array<T, -1, -1>> jac) const -> void {
            auto const len   { static_cast<int64_t>(range.Size()) };
            auto const& polynomials{ individual_.get().GetCoefficients() };
            auto const nn    { std::ssize(polynomials) };
            constexpr int64_t S{ BatchSize };
            auto const rem   { std::min(S, len - row) };

            auto k{jac.cols()};
            for (auto i = nn-1; i >= 0L; --i) {
                auto w = std::get<0>(context_[i]);

//                if (polynomials[i].Optimize) {
//                    jac.col(--k).segment(row, rem) = trace_.col(i).head(rem) * primal_.col(i).head(rem) / w;
//                }

//                for (auto j : Individual::GetFunctions(i)) {
//                    auto const x { static_cast<int64_t>(j) };
//                    trace_.col(x).head(rem) *= trace_.col(i).head(rem) * w;
//                }
            }
        }

        // init individual info into context_ and initializes primal_ columns
        auto InitContext(Turingforge::Span<Turingforge::Scalar const> coeff, Turingforge::Range range) const {
            auto const& polynomials{ individual_.get().GetFunctions() };
            auto const nr { static_cast<int64_t>(range.Size()) };
            auto const nn { std::ssize(polynomials) };

            constexpr int64_t S{ BatchSize };
            primal_ = Eigen::Array<T, S, -1>::Zero(S, nn);
            context_.clear();
            context_.reserve(nn);

            auto const& dt = dtable_.get();
            // aggregate necessary info about the individual into a context object
            for (int64_t i = 0, j = 0; i < nn; ++i) {
                auto const& n = polynomials[i];
                auto const* ptr      = n.IsVariable() ? dataset_.get().GetValues(n.HashValue).subspan(range.Start(), range.Size()).data() : nullptr;
                auto variableValues  = std::tuple_element_t<1, Data>(ptr, nr);
                auto nodeCoefficient = !coeff.empty() ? T{coeff[j++]} : T{n.Value};
                auto nodeFunction    = dt.template TryGetFunction<T>(n.HashValue);
                auto nodeDerivative  = dt.template TryGetDerivative<T>(n.HashValue);

                context_.push_back({ nodeCoefficient, variableValues, nodeFunction, nodeDerivative });

                if (n.IsConstant()) { primal_.col(i).setConstant(nodeCoefficient); }
            }
        }
    };

    // convenience method to interpret many trees in parallel
    auto EvaluateTrees(std::vector<Turingforge::Individual> const& trees, Turingforge::Dataset const& dataset, Turingforge::Range range, size_t nthread = 0) -> std::vector<std::vector<Turingforge::Scalar>>;
    auto EvaluateTrees(std::vector<Turingforge::Individual> const& trees, Turingforge::Dataset const& dataset, Turingforge::Range range, std::span<Turingforge::Scalar> result, size_t nthread = 0) -> void;
} // namespace Turingforge
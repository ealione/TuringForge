#pragma once

#include <Eigen/Core>
#include <vstat/vstat.hpp>

#include "turing_forge/core/types.hpp"
#include "turing_forge/interpreter/interpreter.hpp"

namespace Turingforge {

namespace Concepts {
    template<typename T>
    concept Likelihood = requires(
        Turingforge::Span<Turingforge::Scalar const> x,
        Turingforge::Span<Turingforge::Scalar const> y,
        Turingforge::Span<Turingforge::Scalar const> z
    ) {
        { T::ComputeLikelihood(x, y, z) } -> std::same_as<Turingforge::Scalar>;
        { T::ComputeFisherMatrix(z, y, z) } -> std::convertible_to<Eigen::template Matrix<Turingforge::Scalar, -1, -1>>;
    };
} // namespace Concepts

template <typename T = Turingforge::Scalar>
struct LikelihoodBase {
    using Scalar = T;
    using Matrix = Eigen::Matrix<Scalar, -1, -1>;
    using Vector = Eigen::Matrix<Scalar, -1, 1>;
    using Ref    = Eigen::Ref<Vector>;
    using Cref   = Eigen::Ref<Vector const> const&;

    using scalar_t = T; // for lbfgs solver NOLINT

    explicit LikelihoodBase(Turingforge::InterpreterBase<T> const& interpreter)
        : interpreter_(interpreter)
    {
    }

    [[nodiscard]] auto GetInterpreter() const -> InterpreterBase<Turingforge::Scalar> const& { return interpreter_.get(); }

    // compute function and gradient when called by the optimizer
    [[nodiscard]] virtual auto operator()(Cref, Ref) const noexcept -> Scalar = 0;

    // compute the likelihood value when called standalone
    [[nodiscard]] virtual auto FunctionEvaluations() const -> std::size_t = 0;
    [[nodiscard]] virtual auto JacobianEvaluations() const -> std::size_t = 0;
    [[nodiscard]] virtual auto NumParameters() const -> std::size_t = 0;
    [[nodiscard]] virtual auto NumObservations() const -> std::size_t = 0;

private:
    std::reference_wrapper<Turingforge::InterpreterBase<Turingforge::Scalar> const> interpreter_;
};
} // namespace Turingforge
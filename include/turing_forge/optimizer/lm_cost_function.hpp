#pragma once

#include <Eigen/Core>

#include "turing_forge/interpreter/interpreter.hpp"

namespace Turingforge {

// this cost function is adapted to work with both solvers from Ceres: the normal one and the tiny solver
// for this, a number of template parameters are necessary:
// - the AutodiffCalculator will compute and return the Jacobian matrix
// - the StorageOrder specifies the format of the jacobian (row-major for the big Ceres solver, column-major for the tiny solver)

template<typename T = Turingforge::Scalar, int StorageOrder = Eigen::ColMajor>
struct LMCostFunction {
    static auto constexpr Storage{ StorageOrder };
    using Scalar = Turingforge::Scalar;

    enum {
        NUM_RESIDUALS = Eigen::Dynamic,  // NOLINT
        NUM_PARAMETERS = Eigen::Dynamic, // NOLINT
    };

    explicit LMCostFunction(InterpreterBase<T> const& interpreter, Turingforge::Span<Turingforge::Scalar const> target, Turingforge::Range const range)
        : interpreter_(interpreter)
        , target_{target}
        , range_{range}
        , numResiduals_{range.Size()}
        , numParameters_{static_cast<std::size_t>(interpreter.GetTree().Coefficients.size())}
    { }

    inline auto Evaluate(Scalar const* parameters, Scalar* residuals, Scalar* jacobian) const -> bool // NOLINT
    {
        EXPECT(target_.size() == numResiduals_);
        EXPECT(parameters != nullptr);
        Turingforge::Span<Turingforge::Scalar const> params{ parameters, numParameters_ };

        if (jacobian != nullptr) {
            Turingforge::Span<Turingforge::Scalar> jac{jacobian, static_cast<size_t>(numResiduals_ * numParameters_)};
            interpreter_.get().JacRev(params, range_, jac);
        }

        if (residuals != nullptr) {
            Turingforge::Span<Turingforge::Scalar> res{ residuals, static_cast<size_t>(numResiduals_) };
            interpreter_.get().Evaluate(params, range_, res);
            Eigen::Map<Eigen::Array<Turingforge::Scalar, -1, 1>> x(residuals, numResiduals_);
            Eigen::Map<Eigen::Array<Turingforge::Scalar, -1, 1> const> y(target_.data(), numResiduals_);
            x -= y;
        }
        return true;
    }

    // ceres solver - jacobian must be in row-major format
    // tiny solver - jacobian must be in col-major format
    auto operator()(Scalar const* parameters, Scalar* residuals, Scalar* jacobian) const -> bool
    {
        return Evaluate(parameters, residuals, jacobian);
    }

    [[nodiscard]] auto NumResiduals() const -> int { return numResiduals_; }
    [[nodiscard]] auto NumParameters() const -> int { return numParameters_; }

    // required by Eigen::LevenbergMarquardt
    using JacobianType = Eigen::Matrix<Turingforge::Scalar, -1, -1>;
    using QRSolver     = Eigen::ColPivHouseholderQR<JacobianType>;

    // there is no real documentation but looking at Eigen unit tests, these functions should return zero
    // see: https://gitlab.com/libeigen/eigen/-/blob/master/unsupported/test/NonLinearOptimization.cpp
    auto operator()(Eigen::Matrix<Scalar, -1, 1> const& input, Eigen::Matrix<Scalar, -1, 1>& residual) const -> int
    {
        Evaluate(input.data(), residual.data(), nullptr);
        return 0;
    }

    auto df(Eigen::Matrix<Scalar, -1, 1> const& input, Eigen::Matrix<Scalar, -1, -1>& jacobian) const -> int // NOLINT
    {
        static_assert(StorageOrder == Eigen::ColMajor, "Eigen::LevenbergMarquardt requires the Jacobian to be stored in column-major format.");
        Evaluate(input.data(), nullptr, jacobian.data());
        return 0;
    }

    [[nodiscard]] auto values() const -> int { return NumResiduals(); }  // NOLINT
    [[nodiscard]] auto inputs() const -> int { return NumParameters(); } // NOLINT

private:
    std::reference_wrapper<InterpreterBase<T> const> interpreter_;
    Turingforge::Span<Turingforge::Scalar const> target_;
    Turingforge::Range const range_; // NOLINT
    std::size_t numResiduals_;
    std::size_t numParameters_;
};
} // namespace Turingforge

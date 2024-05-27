#pragma once

#include <atomic>
#include <functional>
#include <utility>

#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/problem.hpp"
#include "turing_forge/core/types.hpp"
#include "turing_forge/interpreter/interpreter.hpp"

namespace Turingforge {

    enum class ErrorType : int { SSE, MSE, NMSE, RMSE, MAE, R2, C2 };

    struct ErrorMetric {
            using Iterator = Turingforge::Span<Turingforge::Scalar const>::iterator;

            explicit ErrorMetric(ErrorType type) : type_(type) { }

            auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double;
            auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double;
            auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double;
            auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double;

            private:
            ErrorType type_;
    };

    struct SSE : public ErrorMetric {
    SSE() : ErrorMetric(ErrorType::SSE) {}
};

struct MSE : public ErrorMetric {
    MSE() : ErrorMetric(ErrorType::MSE) {}
};

struct NMSE : public ErrorMetric {
    NMSE() : ErrorMetric(ErrorType::NMSE) {}
};

struct RMSE : public ErrorMetric {
    RMSE() : ErrorMetric(ErrorType::RMSE) {}
};

struct MAE : public ErrorMetric {
    MAE() : ErrorMetric(ErrorType::MAE) {}
};

struct R2 : public ErrorMetric {
    R2() : ErrorMetric(ErrorType::R2) {}
};

struct C2 : public ErrorMetric {
    C2() : ErrorMetric(ErrorType::C2) {}
};

auto FitLeastSquares(Turingforge::Span<float const> estimated, Turingforge::Span<float const> target) noexcept -> std::pair<double, double>;
auto FitLeastSquares(Turingforge::Span<double const> estimated, Turingforge::Span<double const> target) noexcept -> std::pair<double, double>;

} // namespace Turingforge
#pragma once

#include <atomic>
#include <functional>
#include <utility>

#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/types.hpp"


namespace Turingforge::Operators {

    struct ErrorMetric {
            using Iterator = Turingforge::Span<Turingforge::Scalar const>::iterator;

            ErrorMetric() = default;
            ErrorMetric(ErrorMetric const&) = default;
            ErrorMetric(ErrorMetric&&) = default;
            auto operator=(ErrorMetric const&) -> ErrorMetric& = default;
            auto operator=(ErrorMetric&&) -> ErrorMetric& = default;

            virtual auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double = 0;
            virtual auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double = 0;
            virtual auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double = 0;
            virtual auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double = 0;
            virtual ~ErrorMetric() = default;
    };

    struct SSE: public ErrorMetric {
        auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
        auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
        auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
        auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    struct MSE: public ErrorMetric {
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    struct NMSE: public ErrorMetric {
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    struct RMSE: public ErrorMetric {
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    struct MAE: public ErrorMetric {
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    struct R2: public ErrorMetric {
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    struct C2: public ErrorMetric {
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double override;
    auto operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double override;
    };

    auto FitLeastSquares(Turingforge::Span<float const> estimated, Turingforge::Span<float const> target) noexcept -> std::pair<double, double>;
    auto FitLeastSquares(Turingforge::Span<double const> estimated, Turingforge::Span<double const> target) noexcept -> std::pair<double, double>;

    struct EvaluatorBase : public OperatorBase<Turingforge::Vector<Turingforge::Scalar>, Individual&, Turingforge::Span<Turingforge::Scalar>> {
        mutable std::atomic_ulong ResidualEvaluations { 0 }; // NOLINT
        mutable std::atomic_ulong JacobianEvaluations { 0 }; // NOLINT
        mutable std::atomic_ulong CallCount { 0 }; // NOLINT
        mutable std::atomic_ulong CostFunctionTime { 0 }; // NOLINT
    
        static constexpr size_t DefaultEvaluationBudget = 100'000;
    
        static auto constexpr ErrMax { std::numeric_limits<Turingforge::Scalar>::max() };
    
        using ReturnType = OperatorBase::ReturnType;
    
        explicit EvaluatorBase(Problem& problem)
        : problem_(problem)
                {
                }
    
        virtual void Prepare(Turingforge::Span<Individual const> /*pop*/) const
        {
        }
    
        virtual auto ObjectiveCount() const -> std::size_t { return 1UL; }
    
        auto TotalEvaluations() const -> size_t { return ResidualEvaluations + JacobianEvaluations; }
    
        void SetBudget(size_t value) { budget_ = value; }
        auto Budget() const -> size_t { return budget_; }
        auto BudgetExhausted() const -> bool { return TotalEvaluations() >= Budget(); }
    
        auto Population() const -> Turingforge::Span<Individual const> { return population_; }
        auto SetPopulation(Turingforge::Span<Turingforge::Individual const> pop) const { population_ = pop; }
        auto GetProblem() const -> Problem const& { return problem_; }
        auto GetProblem() -> Problem& { return problem_; }
        auto SetProblem(Problem& problem) { problem_ = problem; }
    
        void Reset()
        {
            ResidualEvaluations = 0;
            JacobianEvaluations = 0;
            CallCount = 0;
            CostFunctionTime = 0;
        }
    
        private:
        mutable Turingforge::Span<Turingforge::Individual const> population_;
        std::reference_wrapper<Problem> problem_;
        size_t budget_ = DefaultEvaluationBudget;
    };

}
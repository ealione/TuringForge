#pragma once

#include "turing_forge/error_metrics/mean_squared_error.hpp"
#include "turing_forge/error_metrics/sum_of_squared_errors.hpp"
#include "turing_forge/interpreter/dispatch_table.hpp"
#include <functional>
#if defined(HAVE_CERES)
#include <ceres/tiny_solver.h>
#else
#include "ceres/tiny_solver.h"
#endif

#include <unsupported/Eigen/LevenbergMarquardt>

#include "dynamic_cost_function.hpp"
#include "likelihood/gaussian_likelihood.hpp"
#include "likelihood/poisson_likelihood.hpp"
#include "lm_cost_function.hpp"
#include "turing_forge/core/comparison.hpp"
#include "turing_forge/core/problem.hpp"
#include "solvers/sgd.hpp"

namespace Turingforge {

enum class OptimizerType : int { Tiny, Eigen, Ceres };

struct OptimizerSummary {
    std::vector<Turingforge::Scalar> InitialParameters;
    std::vector<Turingforge::Scalar> FinalParameters;
    Turingforge::Scalar InitialCost{};
    Turingforge::Scalar FinalCost{};
    int Iterations{};
    int FunctionEvaluations{};
    int JacobianEvaluations{};
    bool Success{};
};

class OptimizerBase {
std::reference_wrapper<Problem const> problem_;
// batch size for loss functions (default = 0 -> use entire data range)
mutable std::size_t batchSize_{0};
mutable std::size_t iterations_{100}; // NOLINT

public:
    explicit OptimizerBase(Problem const& problem)
        : problem_ { problem }
    {
    }

    OptimizerBase(const OptimizerBase&) = default;
    OptimizerBase(OptimizerBase&&) = delete;
    auto operator=(const OptimizerBase&) -> OptimizerBase& = default;
    auto operator=(OptimizerBase&&) -> OptimizerBase& = delete;

    virtual ~OptimizerBase() = default;

    [[nodiscard]] auto GetProblem() const -> Problem const& { return problem_.get(); }
    [[nodiscard]] auto BatchSize() const -> std::size_t { return batchSize_; }
    [[nodiscard]] auto Iterations() const -> std::size_t { return iterations_; }

    auto SetBatchSize(std::size_t batchSize) const { batchSize_ = batchSize; }
    auto SetIterations(std::size_t iterations) const { iterations_ = iterations; }

    [[nodiscard]] virtual auto Optimize(Turingforge::RandomGenerator& rng, Individual const& individual) const -> OptimizerSummary = 0;
    [[nodiscard]] virtual auto ComputeLikelihood(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const -> Turingforge::Scalar = 0;
    [[nodiscard]] virtual auto ComputeFisherMatrix(Turingforge::Span<Turingforge::Scalar const> pred, Turingforge::Span<Turingforge::Scalar const> jac, Turingforge::Span<Turingforge::Scalar const> sigma) const -> Eigen::Matrix<Turingforge::Scalar, -1, -1> = 0;
};

namespace detail {
    inline auto CheckSuccess(double initialCost, double finalCost) {
        constexpr auto CHECK_NAN{true};
        return Turingforge::Less<CHECK_NAN>{}(finalCost, initialCost);
    }
} // namespace detail

template <typename DTable, OptimizerType = OptimizerType::Tiny>
struct LevenbergMarquardtOptimizer : public OptimizerBase {
    explicit LevenbergMarquardtOptimizer(DTable const& dtable, Problem const& problem)
        : OptimizerBase{problem}, dtable_{dtable}
    {
    }

    [[nodiscard]] auto Optimize(Turingforge::RandomGenerator& /*unused*/, Turingforge::Individual const& individual) const -> OptimizerSummary final
    {
        auto const& dtable = this->GetDispatchTable();
        auto const& problem = this->GetProblem();
        auto const& dataset = problem.GetDataset();
        auto range  = problem.TrainingRange();
        auto target = problem.TargetValues(range);
        auto iterations = this->Iterations();

        Turingforge::Interpreter<Turingforge::Scalar, DTable> interpreter{dtable, dataset, individual};
        Turingforge::LMCostFunction cf{interpreter, target, range};
        ceres::TinySolver<decltype(cf)> solver;
        solver.options.max_num_iterations = static_cast<int>(iterations);

        auto x0 = individual.GetCoefficients();
        OptimizerSummary summary;
        summary.InitialParameters = x0;
        auto m0 = Eigen::Map<Eigen::Matrix<Turingforge::Scalar, Eigen::Dynamic, 1>>(x0.data(), x0.size());
        if (!x0.empty()) {
            typename decltype(solver)::Parameters p = m0.cast<typename decltype(cf)::Scalar>();
            solver.Solve(cf, &p);
            m0 = p.template cast<Turingforge::Scalar>();
        }
        summary.FinalParameters = x0;
        summary.InitialCost = solver.summary.initial_cost;
        summary.FinalCost = solver.summary.final_cost;
        summary.Iterations = solver.summary.iterations;
        summary.FunctionEvaluations = solver.summary.iterations;
        summary.Success = detail::CheckSuccess(summary.InitialCost, summary.FinalCost);
        return summary;
    }

    auto GetDispatchTable() const -> DTable const& { return dtable_.get(); }

    [[nodiscard]] auto ComputeLikelihood(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const -> Turingforge::Scalar final
    {
        return GaussianLikelihood<Turingforge::Scalar>::ComputeLikelihood(x, y, w);
    }

    [[nodiscard]] auto ComputeFisherMatrix(Turingforge::Span<Turingforge::Scalar const> pred, Turingforge::Span<Turingforge::Scalar const> jac, Turingforge::Span<Turingforge::Scalar const> sigma) const -> Eigen::Matrix<Turingforge::Scalar, -1, -1> final {
        return GaussianLikelihood<Turingforge::Scalar>::ComputeFisherMatrix(pred, jac, sigma);
    }

    private:
    std::reference_wrapper<DTable const> dtable_;
};

template <typename DTable>
struct LevenbergMarquardtOptimizer<DTable, OptimizerType::Eigen> final : public OptimizerBase {
    explicit LevenbergMarquardtOptimizer(DTable const& dtable, Problem const& problem)
            : OptimizerBase{problem}, dtable_{dtable}
    {
    }

    [[nodiscard]] auto Optimize(Turingforge::RandomGenerator& /*unused*/, Turingforge::Individual const& individual) const -> OptimizerSummary final
    {
        auto const& dtable = this->GetDispatchTable();
        auto const& problem = this->GetProblem();
        auto const& dataset = problem.GetDataset();
        auto range  = problem.TrainingRange();
        auto target = problem.TargetValues(range);
        auto iterations = this->Iterations();

        Turingforge::Interpreter<Turingforge::Scalar, DTable> interpreter{dtable, dataset, individual};
        Turingforge::LMCostFunction<Turingforge::Scalar> cf{interpreter, target, range};
        Eigen::LevenbergMarquardt<decltype(cf)> lm(cf);
        lm.setMaxfev(static_cast<int>(iterations+1));

        auto x0 = individual.GetCoefficients();
        OptimizerSummary summary;
        summary.InitialParameters = x0;
        if (!x0.empty()) {
            Eigen::Map<Eigen::Matrix<Turingforge::Scalar, -1, 1>> m0(x0.data(), std::ssize(x0));
            Eigen::Matrix<Turingforge::Scalar, -1, 1> m = m0;

            // do the minimization loop manually because we want to extract the initial cost
            Eigen::LevenbergMarquardtSpace::Status status = lm.minimizeInit(m);
            summary.InitialCost = summary.FinalCost = lm.fnorm() * lm.fnorm() * 0.5; // get the initial cost after calling minimizeInit()
            if (status != Eigen::LevenbergMarquardtSpace::ImproperInputParameters) {
                do {
                    status = lm.minimizeOneStep(m);
                } while (status == Eigen::LevenbergMarquardtSpace::Running);
            }
            m0 = m;
        }
        summary.FinalParameters = x0;
        summary.FinalCost = lm.fnorm() * lm.fnorm();
        summary.Iterations = static_cast<int>(lm.iterations());
        summary.FunctionEvaluations = static_cast<int>(lm.nfev());
        summary.JacobianEvaluations = static_cast<int>(lm.njev());
        summary.Success = detail::CheckSuccess(summary.InitialCost, summary.FinalCost);
        return summary;
    }

    auto GetDispatchTable() const -> DTable const& { return dtable_.get(); }

    [[nodiscard]] auto ComputeLikelihood(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const -> Turingforge::Scalar final
    {
        return GaussianLikelihood<Turingforge::Scalar>::ComputeLikelihood(x, y, w);
    }

    [[nodiscard]] auto ComputeFisherMatrix(Turingforge::Span<Turingforge::Scalar const> pred, Turingforge::Span<Turingforge::Scalar const> jac, Turingforge::Span<Turingforge::Scalar const> sigma) const -> Eigen::Matrix<Turingforge::Scalar, -1, -1> final {
        return GaussianLikelihood<Turingforge::Scalar>::ComputeFisherMatrix(pred, jac, sigma);
    }

private:
    std::reference_wrapper<DTable const> dtable_;
};

#if defined(HAVE_CERES)
template <typename T = Turingforge::Scalar>
struct NonlinearLeastSquaresOptimizer<T, OptimizerType::Ceres> : public OptimizerBase<T> {
explicit NonlinearLeastSquaresOptimizer(InterpreterBase<T>& interpreter)
    : OptimizerBase<T>{interpreter}
{
}

auto Optimize(Turingforge::Span<Turingforge::Scalar const> target, Range range, size_t iterations, OptimizerSummary& summary) -> std::vector<Turingforge::Scalar> final
{
    auto const& individual = this->GetTree();
    auto const& ds = this->GetDataset();
    auto const& dt = this->GetDispatchTable();

    auto x0 = individual.GetCoefficients();

    Turingforge::CostFunction<DTable, Eigen::RowMajor> cf(individual, ds, target, range, dt);
    auto costFunction = new Turingforge::DynamicCostFunction(cf); // NOLINT

    ceres::Solver::Summary s;
    if (!x0.empty()) {
        Eigen::Map<Eigen::Matrix<Turingforge::Scalar, -1, 1>> m0(x0.data(), std::ssize(x0));
        auto sz = static_cast<Eigen::Index>(x0.size());
        Eigen::VectorXd params = Eigen::Map<Eigen::Matrix<Turingforge::Scalar, -1, 1>>(x0.data(), sz).template cast<double>();
        ceres::Problem problem;
        problem.AddResidualBlock(costFunction, nullptr, params.data());
        ceres::Solver::Options options;
        options.linear_solver_type = ceres::DENSE_QR;
        options.logging_type = ceres::LoggingType::SILENT;
        options.max_num_iterations = static_cast<int>(iterations - 1); // workaround since for some reason ceres sometimes does 1 more iteration
        options.minimizer_progress_to_stdout = false;
        options.num_threads = 1;
        options.trust_region_strategy_type = ceres::LEVENBERG_MARQUARDT;
        options.use_inner_iterations = false;
        Solve(options, &problem, &s);
        m0 = params.cast<Turingforge::Scalar>();
    }
    summary.InitialCost = s.initial_cost;
    summary.FinalCost = s.final_cost;
    summary.Iterations = static_cast<int>(s.iterations.size());
    summary.FunctionEvaluations = s.num_residual_evaluations;
    summary.JacobianEvaluations = s.num_jacobian_evaluations;
    summary.Success = detail::CheckSuccess(summary.InitialCost, summary.FinalCost);
    return x0;
}
};
#endif

template<typename DTable, Concepts::Likelihood LossFunction = GaussianLikelihood<Turingforge::Scalar>>
struct SGDOptimizer final : public OptimizerBase {
    SGDOptimizer(DTable const& dtable, Problem const& problem)
        : OptimizerBase{problem}
        , dtable_{dtable}
        , update_{std::make_unique<UpdateRule::Constant<Turingforge::Scalar>>(Turingforge::Scalar{0.01})}
    { }

    SGDOptimizer(DTable const& dtable, Problem const& problem, UpdateRule::LearningRateUpdateRule const& update)
        : OptimizerBase{problem}
        , dtable_{dtable}
        , update_{update.Clone(0)}
    { }

    auto GetDispatchTable() const -> DTable const& { return dtable_.get(); }

    [[nodiscard]] auto Optimize(Turingforge::RandomGenerator& rng, Turingforge::Individual const& individual) const -> OptimizerSummary final
    {
        auto const& dtable = this->GetDispatchTable();
        auto const& problem = this->GetProblem();
        auto const& dataset = problem.GetDataset();
        auto range  = problem.TrainingRange();
        auto target = problem.TargetValues(range);
        auto iterations = this->Iterations();
        auto batchSize = this->BatchSize();
        if (batchSize == 0) { batchSize = range.Size(); }

        Turingforge::Interpreter<Turingforge::Scalar, DTable> interpreter{dtable, dataset, individual};
        LossFunction loss{rng, interpreter, target, range, batchSize};

        auto cost = [&]() {
            auto pred = interpreter.Evaluate(range);
            return 0.5 * Turingforge::SumOfSquaredErrors(pred.begin(), pred.end(), target.begin());
        };

        auto coeff = individual.GetCoefficients();
        auto const f0 = cost();
        OptimizerSummary summary;
        summary.InitialParameters = coeff;
        summary.InitialCost = f0;
        auto rule = update_->Clone(coeff.size());
        SGDSolver<LossFunction> solver(loss, *rule);

        Eigen::Map<Eigen::Array<Turingforge::Scalar, -1, 1> const> x0(coeff.data(), std::ssize(coeff));
        auto x = solver.Optimize(x0, iterations);
        std::copy(x.begin(), x.end(), coeff.begin());
        auto const f1 = cost();

        summary.FinalParameters = coeff;
        summary.FinalCost = f1;
        summary.Success = detail::CheckSuccess(f0, f1);
        summary.Iterations = solver.Epochs();
        auto const funEvals = loss.FunctionEvaluations();
        auto const jacEvals = loss.JacobianEvaluations();
        auto const rangeSize = range.Size();
        summary.FunctionEvaluations = static_cast<std::size_t>(static_cast<double>(funEvals + jacEvals) * batchSize / rangeSize);
        summary.JacobianEvaluations = summary.FunctionEvaluations;
        return summary;
    }

    [[nodiscard]] virtual auto ComputeLikelihood(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const -> Turingforge::Scalar
    {
        return LossFunction::ComputeLikelihood(x, y, w);
    }

    [[nodiscard]] virtual auto ComputeFisherMatrix(Turingforge::Span<Turingforge::Scalar const> pred, Turingforge::Span<Turingforge::Scalar const> jac, Turingforge::Span<Turingforge::Scalar const> sigma) const -> Eigen::Matrix<Turingforge::Scalar, -1, -1> final {
        return LossFunction::ComputeFisherMatrix(pred, jac, sigma);
    }

    auto SetUpdateRule(std::unique_ptr<UpdateRule::LearningRateUpdateRule const> update) {
        update_ = std::move(update);
    }

    auto UpdateRule() const { return update_.get(); }

    private:
    std::reference_wrapper<DTable const> dtable_;
    std::unique_ptr<UpdateRule::LearningRateUpdateRule const> update_{nullptr};
};
} // namespace Turingforge
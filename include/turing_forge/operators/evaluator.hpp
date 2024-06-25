#pragma once

#include <atomic>
#include <functional>
#include <utility>

#include "turing_forge/collections/projection.hpp"
#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/problem.hpp"
#include "turing_forge/core/types.hpp"
#include "turing_forge/interpreter/interpreter.hpp"

namespace Turingforge {

    enum class ErrorType : int {
        SSE, MSE, NMSE, RMSE, MAE, R2, C2
    };

    struct ErrorMetric {
        using Iterator = Turingforge::Span<Turingforge::Scalar const>::iterator;
        using ProjIterator = ProjectionIterator<Iterator>;

        explicit ErrorMetric(ErrorType type) : type_(type) {}

        auto operator()(Turingforge::Span<Turingforge::Scalar const> x,
                        Turingforge::Span<Turingforge::Scalar const> y) const -> double;

        auto operator()(Turingforge::Span<Turingforge::Scalar const> x,
                        Turingforge::Span<Turingforge::Scalar const> y,
                        Turingforge::Span<Turingforge::Scalar const> w) const -> double;

        auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const -> double;

        auto operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const -> double;

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

    auto FitLeastSquares(Turingforge::Span<float const> estimated,
                         Turingforge::Span<float const> target) noexcept -> std::pair<double, double>;

    auto FitLeastSquares(Turingforge::Span<double const> estimated,
                         Turingforge::Span<double const> target) noexcept -> std::pair<double, double>;

    struct EvaluatorBase : public OperatorBase<Turingforge::Vector<Turingforge::Scalar>, Individual &, Turingforge::Span<Turingforge::Scalar>> {
        mutable std::atomic_ulong ResidualEvaluations{0}; // NOLINT
        mutable std::atomic_ulong JacobianEvaluations{0}; // NOLINT
        mutable std::atomic_ulong CallCount{0}; // NOLINT
        mutable std::atomic_ulong CostFunctionTime{0}; // NOLINT

        static constexpr size_t DefaultEvaluationBudget = 100'000;

        static auto constexpr ErrMax{std::numeric_limits<Turingforge::Scalar>::max()};

        using ReturnType = OperatorBase::ReturnType;

        explicit EvaluatorBase(Problem &problem)
                : problem_(problem) {
        }

        virtual void Prepare(Turingforge::Span<Individual const> /*pop*/) const {
        }

        virtual auto ObjectiveCount() const -> std::size_t { return 1UL; }

        auto TotalEvaluations() const -> size_t { return ResidualEvaluations + JacobianEvaluations; }

        void SetBudget(size_t value) { budget_ = value; }

        auto Budget() const -> size_t { return budget_; }

        // virtual because more complex evaluators (e.g. MultiEvaluator) might need to calculate it differently
        virtual auto BudgetExhausted() const -> bool { return TotalEvaluations() >= Budget(); }

        virtual auto Stats() const -> std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> {
            return std::tuple{
                    ResidualEvaluations.load(),
                    JacobianEvaluations.load(),
                    CallCount.load(),
                    CostFunctionTime.load()
            };
        }

        auto Population() const -> Turingforge::Span<Individual const> { return population_; }

        auto SetPopulation(Turingforge::Span<Turingforge::Individual const> pop) const { population_ = pop; }

        auto GetProblem() const -> Problem const & { return problem_; }

        auto GetProblem() -> Problem & { return problem_; }

        auto SetProblem(Problem &problem) { problem_ = problem; }

        void Reset() {
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

    class UserDefinedEvaluator : public EvaluatorBase {
    public:
        UserDefinedEvaluator(Problem &problem, std::function<typename EvaluatorBase::ReturnType(Turingforge::RandomGenerator &, Turingforge::Individual &)> func)
                : EvaluatorBase(problem), fref_(std::move(func)) {
        }

        // the func signature taking a pointer to the rng is a workaround for pybind11, since the random generator is non-copyable we have to pass a pointer
        UserDefinedEvaluator(Problem &problem, std::function<typename EvaluatorBase::ReturnType(Turingforge::RandomGenerator *, Turingforge::Individual &)> func)
                : EvaluatorBase(problem), fptr_(std::move(func)) {
        }

        auto
        operator()(Turingforge::RandomGenerator &rng, Individual &ind, Turingforge::Span<Turingforge::Scalar> /*args*/) const -> typename EvaluatorBase::ReturnType override {
            ++this->CallCount;
            return fptr_ ? fptr_(&rng, ind) : fref_(rng, ind);
        }

    private:
        std::function<typename EvaluatorBase::ReturnType(Turingforge::RandomGenerator &, Turingforge::Individual &)> fref_;
        std::function<typename EvaluatorBase::ReturnType(Turingforge::RandomGenerator *, Turingforge::Individual &)> fptr_; // workaround for pybind11
    };

    template<typename DTable>
    class Evaluator : public EvaluatorBase {
        using TInterpreter = Turingforge::Interpreter<Turingforge::Scalar, DTable>;

    public:
        explicit Evaluator(Problem &problem, DTable const &dtable, ErrorMetric error = MSE{}, bool linearScaling = true)
                : EvaluatorBase(problem), dtable_(dtable), error_(error), scaling_(linearScaling) {
        }

        auto GetDispatchTable() const { return dtable_.get(); }

        auto
        operator()(Turingforge::RandomGenerator & /*random*/,
                   Individual &ind,
                   Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType override;

    private:
        std::reference_wrapper<DTable const> dtable_;
        ErrorMetric error_;
        bool scaling_{false};
    };

    class MultiEvaluator : public EvaluatorBase {
    public:
        explicit MultiEvaluator(Problem &problem) : EvaluatorBase(problem) {
        }

        auto Add(EvaluatorBase const &evaluator) {
            evaluators_.push_back(std::ref(evaluator));
        }

        auto Prepare(Turingforge::Span<Turingforge::Individual const> pop) const -> void override {
            for (auto const &e: evaluators_) {
                e.get().Prepare(pop);
            }
        }

        auto ObjectiveCount() const -> std::size_t override {
            return std::transform_reduce(evaluators_.begin(), evaluators_.end(), 0UL, std::plus{},
                                         [](auto const &eval) { return eval.get().ObjectiveCount(); });
        }

        auto
        operator()(Turingforge::RandomGenerator &rng, Individual &ind,
                   Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType override {
            EvaluatorBase::ReturnType fit;
            fit.reserve(ind.Size());

            for (auto const &ev: evaluators_) {
                auto f = ev(rng, ind, buf);
                std::copy(f.begin(), f.end(), std::back_inserter(fit));
            }

            return fit;
        }

        auto Stats() const -> std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> final {
            auto resEval{0UL};
            auto jacEval{0UL};
            auto callCnt{0UL};
            auto cfTime{0UL};

            for (auto const &eval: evaluators_) {
                auto [re, je, cc, ct] = eval.get().Stats();
                resEval += re;
                jacEval += je;
                callCnt += cc;
                cfTime += ct;
            }

            return std::tuple{resEval + ResidualEvaluations.load(),
                              jacEval + JacobianEvaluations.load(),
                              callCnt + CallCount.load(),
                              cfTime + CostFunctionTime.load()};
        }

        auto BudgetExhausted() const -> bool final {
            auto [re, je, cc, ct] = Stats();
            return re + je >= Budget();
        }

        auto Evaluators() const { return evaluators_; }

    private:
        std::vector<std::reference_wrapper<EvaluatorBase const>> evaluators_;
    };

    class AggregateEvaluator final : public EvaluatorBase {
    public:
        enum class AggregateType : int {
            Min,
            Max,
            Median,
            Mean,
            HarmonicMean,
            Sum
        };

        explicit AggregateEvaluator(EvaluatorBase &evaluator)
                : EvaluatorBase(evaluator.GetProblem()), evaluator_(evaluator) {
        }

        auto SetAggregateType(AggregateType type) { aggtype_ = type; }

        auto GetAggregateType() const { return aggtype_; }

        auto
        operator()(Turingforge::RandomGenerator &rng, Individual &ind,
                   Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType override;

    private:
        std::reference_wrapper<EvaluatorBase const> evaluator_;
        AggregateType aggtype_{AggregateType::Mean};
    };

} // namespace Turingforge
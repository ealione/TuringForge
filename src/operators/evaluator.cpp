#include "turing_forge/core/distance.hpp"
#include "turing_forge/error_metrics/error_metrics.hpp"
#include "turing_forge/formatter/formatter.hpp"
#include "turing_forge/interpreter/dispatch_table.hpp"
#include "turing_forge/interpreter/interpreter.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/optimizer/likelihood/gaussian_likelihood.hpp"
#include "turing_forge/optimizer/optimizer.hpp"
#include "turing_forge/optimizer/solvers/sgd.hpp"

#include <chrono>
#include <ranges>
#include <type_traits>

namespace Turingforge {
    template<typename T>
    auto FitLeastSquaresImpl(Turingforge::Span<T const> estimated,
                             Turingforge::Span<T const> target) -> std::pair<double, double>requires std::is_arithmetic_v<T> {
        auto stats = vstat::bivariate::accumulate<T>(std::cbegin(estimated), std::cend(estimated), std::cbegin(target));
        auto a = stats.covariance / stats.variance_x; // scale
        if (!std::isfinite(a)) {
            a = 1;
        }
        auto b = stats.mean_y - a * stats.mean_x; // offset
        return {a, b};
    }

    auto FitLeastSquares(Turingforge::Span<float const> estimated, Turingforge::Span<float const> target) noexcept -> std::pair<double, double> {
        return FitLeastSquaresImpl<float>(estimated, target);
    }

    auto FitLeastSquares(Turingforge::Span<double const> estimated, Turingforge::Span<double const> target) noexcept -> std::pair<double, double> {
        return FitLeastSquaresImpl<double>(estimated, target);
    }

    template<>
    auto
    Evaluator<DefaultDispatch>::operator()(Turingforge::RandomGenerator & /*rng*/,
                                           Individual &ind,
                                           Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType {
        ++CallCount;
        auto const &problem = GetProblem();
        auto const &dataset = problem.GetDataset();

        auto trainingRange = problem.TrainingRange();
        auto targetValues = dataset.GetValues(problem.TargetVariable()).subspan(trainingRange.Start(), trainingRange.Size());

        auto const &dtable = GetDispatchTable();
        TInterpreter const interpreter{dtable, dataset, ind};

        auto computeFitness = [&]() {
            ++ResidualEvaluations;
            Turingforge::Vector<Turingforge::Scalar> estimatedValues;
            if (buf.size() != trainingRange.Size()) {
                estimatedValues.resize(trainingRange.Size());
                buf = {estimatedValues.data(), estimatedValues.size()};
            }
            auto coeff = ind.GetCoefficients();
            interpreter.Evaluate(coeff, trainingRange, buf);
            if (scaling_) {
                auto [a, b] = FitLeastSquaresImpl<Turingforge::Scalar>(buf, targetValues);
                std::transform(buf.begin(), buf.end(), buf.begin(), [a = a, b = b](auto x) { return a * x + b; });
            }
            ENSURE(buf.size() >= targetValues.size());
            return error_(buf, targetValues);
        };

        auto fit = static_cast<Turingforge::Scalar>(computeFitness());
        if (!std::isfinite(fit)) {
            fit = EvaluatorBase::ErrMax;
        }
        return typename EvaluatorBase::ReturnType{fit};
    }

    auto
    AggregateEvaluator::operator()(Turingforge::RandomGenerator &rng, Individual &ind,
                                   Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType {
        using vstat::univariate::accumulate;
        auto f = evaluator_.get()(rng, ind, buf);
        switch (aggtype_) {
            case AggregateType::Min: {
                return {*std::min_element(f.begin(), f.end())};
            }
            case AggregateType::Max: {
                return {*std::max_element(f.begin(), f.end())};
            }
            case AggregateType::Median: {
                auto const sz{std::ssize(f)};
                auto const a = f.begin() + sz / 2;
                std::nth_element(f.begin(), a, f.end());
                if (sz % 2 == 0) {
                    auto const b = std::max_element(f.begin(), a);
                    return {(*a + *b) / 2};
                }
                return {*a};
            }
            case AggregateType::Mean: {
                return {static_cast<Turingforge::Scalar>(accumulate<Turingforge::Scalar>(f.begin(), f.end()).mean)};
            }
            case AggregateType::HarmonicMean: {
                auto stats = accumulate<Turingforge::Scalar>(f.begin(), f.end(), [](auto x) { return 1 / x; });
                return {static_cast<Turingforge::Scalar>(stats.count / stats.sum)};
            }
            case AggregateType::Sum: {
                return {static_cast<Turingforge::Scalar>(vstat::univariate::accumulate<Turingforge::Scalar>(f.begin(),
                                                                                                            f.end()).sum)};
            }
            default: {
                throw std::runtime_error("Unknown AggregateType");
            }
        }
    }
} // namespace Turingforge
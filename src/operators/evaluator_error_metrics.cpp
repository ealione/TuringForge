#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/error_metrics/error_metrics.hpp"

namespace Turingforge {
    auto ErrorMetric::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double {
        switch (type_) {
            case ErrorType::SSE: return SumOfSquaredErrors(x, y);
            case ErrorType::MSE: return MeanSquaredError(x, y);
            case ErrorType::NMSE: return NormalizedMeanSquaredError(x, y);
            case ErrorType::RMSE: return RootMeanSquaredError(x, y);
            case ErrorType::MAE: return MeanAbsoluteError(x, y);
            case ErrorType::R2: return -R2Score(x, y);
            case ErrorType::C2: return -SquaredCorrelation(x, y);
            default: return 0.0;
        }
    }

    auto ErrorMetric::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double {
        switch (type_) {
            case ErrorType::SSE: return SumOfSquaredErrors(x, y, w);
            case ErrorType::MSE: return MeanSquaredError(x, y, w);
            case ErrorType::NMSE: return NormalizedMeanSquaredError(x, y, w);
            case ErrorType::RMSE: return RootMeanSquaredError(x, y, w);
            case ErrorType::MAE: return MeanAbsoluteError(x, y, w);
            case ErrorType::R2: return -R2Score(x, y, w);
            case ErrorType::C2: break; //return -SquaredCorrelation(x, y, w);
            default: return 0.0;
        }
        return 0.0;
    }

    auto ErrorMetric::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double {
        switch (type_) {
            case ErrorType::SSE: return SumOfSquaredErrors(beg1, end1, beg2);
            case ErrorType::MSE: return MeanSquaredError(beg1, end1, beg2);
            case ErrorType::NMSE: return NormalizedMeanSquaredError(beg1, end1, beg2);
            case ErrorType::RMSE: return RootMeanSquaredError(beg1, end1, beg2);
            case ErrorType::MAE: return MeanAbsoluteError(beg1, end1, beg2);
            case ErrorType::R2: return -R2Score(beg1, end1, beg2);
            case ErrorType::C2: return -SquaredCorrelation(beg1, end1, beg2);
            default: return 0.0;
        }
    }

    auto ErrorMetric::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double {
        switch (type_) {
            case ErrorType::SSE: return SumOfSquaredErrors(beg1, end1, beg2, beg3);
            case ErrorType::MSE: return MeanSquaredError(beg1, end1, beg2, beg3);
            case ErrorType::NMSE: return NormalizedMeanSquaredError(beg1, end1, beg2, beg3);
            case ErrorType::RMSE: return RootMeanSquaredError(beg1, end1, beg2, beg3);
            case ErrorType::MAE: return MeanAbsoluteError(beg1, end1, beg2, beg3);
            case ErrorType::R2: return -R2Score(beg1, end1, beg2, beg3);
            case ErrorType::C2: break; //return -SquaredCorrelation(beg1, end1, beg2, beg3);
            default: return 0.0;
        }
        return 0.0;
    }
}  // namespace Turingforge
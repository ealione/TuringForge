//#include "turing_forge/core/distance.hpp"
//#include "turing_forge/error_metrics/error_metrics.hpp"
//#include "turing_forge/formatter/formatter.hpp"
//#include "turing_forge/interpreter/dispatch_table.hpp"
//#include "turing_forge/interpreter/interpreter.hpp"
//#include "turing_forge/operators/evaluator.hpp"
//#include "turing_forge/optimizer/likelihood/gaussian_likelihood.hpp"
//#include "turing_forge/optimizer/optimizer.hpp"
//#include "turing_forge/optimizer/solvers/sgd.hpp"
//
//#include <turing_forge/operon_export.hpp>
//#include <taskflow/taskflow.hpp>
//#include <chrono>
//#include <ranges>
//#include <type_traits>
//
//namespace Turingforge {
//    auto Operators::SSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        return SumOfSquaredErrors(x.begin(), x.end(), y.begin());
//    }
//
//    auto Operators::SSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        return SumOfSquaredErrors(x.begin(), x.end(), y.begin(), w.begin());
//    }
//
//    auto Operators::SSE::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        return SumOfSquaredErrors(beg1, end1, beg2);
//    }
//
//    auto Operators::SSE::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        return SumOfSquaredErrors(beg1, end1, beg2, beg3);
//    }
//
//    auto Operators::MSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        return MeanSquaredError(x.begin(), x.end(), y.begin());
//    }
//
//    auto Operators::MSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        return MeanSquaredError(x.begin(), x.end(), y.begin(), w.begin());
//    }
//
//    auto Operators::MSE::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        return MeanSquaredError(beg1, end1, beg2);
//    }
//
//    auto Operators::MSE::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        return MeanSquaredError(beg1, end1, beg2, beg3);
//    }
//
//    auto Operators::RMSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        return RootMeanSquaredError(x.begin(), x.end(), y.begin());
//    }
//
//    auto Operators::RMSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        return RootMeanSquaredError(x.begin(), x.end(), y.begin(), w.begin());
//    }
//
//    auto Operators::RMSE::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        return RootMeanSquaredError(beg1, end1, beg2);
//    }
//
//    auto Operators::RMSE::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        return RootMeanSquaredError(beg1, end1, beg2, beg3);
//    }
//
//    auto Operators::NMSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        return NormalizedMeanSquaredError(x.begin(), x.end(), y.begin());
//    }
//
//    auto Operators::NMSE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        return NormalizedMeanSquaredError(x.begin(), x.end(), y.begin(), w.begin());
//    }
//
//    auto Operators::NMSE::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        return NormalizedMeanSquaredError(beg1, end1, beg2);
//    }
//
//    auto Operators::NMSE::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        return NormalizedMeanSquaredError(beg1, end1, beg2, beg3);
//    }
//
//    auto Operators::MAE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        return MeanAbsoluteError(x.begin(), x.end(), y.begin());
//    }
//
//    auto Operators::MAE::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        return MeanAbsoluteError(x.begin(), x.end(), y.begin(), w.begin());
//    }
//
//    auto Operators::MAE::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        return MeanAbsoluteError(beg1, end1, beg2);
//    }
//
//    auto Operators::MAE::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        return MeanAbsoluteError(beg1, end1, beg2, beg3);
//    }
//
//    auto Operators::R2::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        return -R2Score(x.begin(), x.end(), y.begin());
//    }
//
//    auto Operators::R2::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        return -R2Score(x.begin(), x.end(), y.begin(), w.begin());
//    }
//
//    auto Operators::R2::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        return -R2Score(beg1, end1, beg2);
//    }
//
//    auto Operators::R2::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        return -R2Score(beg1, end1, beg2, beg3);
//    }
//
//    auto Operators::C2::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y) const noexcept -> double
//    {
//        auto r = CorrelationCoefficient(x.begin(), x.end(), y.begin());
//        return -(r * r);
//    }
//
//    auto Operators::C2::operator()(Turingforge::Span<Turingforge::Scalar const> x, Turingforge::Span<Turingforge::Scalar const> y, Turingforge::Span<Turingforge::Scalar const> w) const noexcept -> double
//    {
//        auto r = CorrelationCoefficient(x.begin(), x.end(), y.begin(), w.begin());
//        return -(r * r);
//    }
//
//    auto Operators::C2::operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double
//    {
//        auto r = CorrelationCoefficient(beg1, end1, beg2);
//        return -(r * r);
//    }
//
//    auto Operators::C2::operator()(Iterator beg1, Iterator end1, Iterator beg2, Iterator beg3) const noexcept -> double
//    {
//        auto r = CorrelationCoefficient(beg1, end1, beg2, beg3);
//        return -(r * r);
//    }
//
//    template<typename T>
//    auto FitLeastSquaresImpl(Turingforge::Span<T const> estimated, Turingforge::Span<T const> target) -> std::pair<double, double>
//    requires std::is_arithmetic_v<T>
//    {
//        auto stats = vstat::bivariate::accumulate<T>(std::cbegin(estimated), std::cend(estimated), std::cbegin(target));
//        auto a = stats.covariance / stats.variance_x; // scale
//        if (!std::isfinite(a)) {
//            a = 1;
//        }
//        auto b = stats.mean_y - a * stats.mean_x; // offset
//        return {a, b};
//    }
//
//    auto FitLeastSquares(Turingforge::Span<float const> estimated, Turingforge::Span<float const> target) noexcept -> std::pair<double, double> {
//        return FitLeastSquaresImpl<float>(estimated, target);
//    }
//
//    auto FitLeastSquares(Turingforge::Span<double const> estimated, Turingforge::Span<double const> target) noexcept -> std::pair<double, double> {
//        return FitLeastSquaresImpl<double>(estimated, target);
//    }
//
//    template<>
//    Evaluator<DefaultDispatch>::operator()(Turingforge::RandomGenerator& rng, Individual& ind, Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType
//    {
//        ++CallCount;
//        auto const& problem = GetProblem();
//        auto const& dataset = problem.GetDataset();
//
//        auto trainingRange = problem.TrainingRange();
//        auto targetValues = dataset.GetValues(problem.TargetVariable()).subspan(trainingRange.Start(), trainingRange.Size());
//
//        auto& tree = ind.Genotype;
//        auto const& dtable = GetDispatchTable();
//        TInterpreter const interpreter{dtable, dataset, tree};
//
//        auto computeFitness = [&]() {
//            ++ResidualEvaluations;
//            Turingforge::Vector<Turingforge::Scalar> estimatedValues;
//            if (buf.size() != trainingRange.Size()) {
//                estimatedValues.resize(trainingRange.Size());
//                buf = { estimatedValues.data(), estimatedValues.size() };
//            }
//            auto coeff = tree.GetCoefficients();
//            interpreter.Evaluate(coeff, trainingRange, buf);
//            if (scaling_) {
//                auto [a, b] = FitLeastSquaresImpl<Turingforge::Scalar>(buf, targetValues);
//                std::transform(buf.begin(), buf.end(), buf.begin(), [a=a,b=b](auto x) { return a * x + b; });
//            }
//            ENSURE(buf.size() >= targetValues.size());
//            return error_(buf, targetValues);
//        };
//
//        if (optimizer_ != nullptr && optimizer_->Iterations() > 0) {
//            auto t0 = std::chrono::steady_clock::now();
//            auto summary = optimizer_->Optimize(rng, tree);
//            // auto coefficients = opt.Optimize(targetValues, trainingRange, iter, summary);
//            ResidualEvaluations += summary.FunctionEvaluations;
//            JacobianEvaluations += summary.JacobianEvaluations;
//            if (summary.Success) { tree.SetCoefficients(summary.FinalParameters); }
//            auto t1 = std::chrono::steady_clock::now();
//            CostFunctionTime += std::chrono::duration_cast<std::chrono::microseconds>(t1-t0).count();
//        }
//
//        auto fit = static_cast<Turingforge::Scalar>(computeFitness());
//        if (!std::isfinite(fit)) {
//            fit = EvaluatorBase::ErrMax;
//        }
//        return typename EvaluatorBase::ReturnType{ fit };
//    }
//
//    auto DiversityEvaluator::Prepare(Turingforge::Span<Turingforge::Individual const> pop) const -> void {
//        divmap_.clear();
//        for (auto const& individual : pop) {
//            auto const& tree = individual.Genotype;
//            auto const& nodes = tree.Nodes();
//            (void) tree.Hash(hashmode_);
//            std::vector<Turingforge::Hash> hash(nodes.size());;
//            std::transform(nodes.begin(), nodes.end(), hash.begin(), [](auto const& n) { return n.CalculatedHashValue; });
//            std::stable_sort(hash.begin(), hash.end());
//            divmap_[tree.HashValue()] = std::move(hash);
//        }
//    }
//
//    auto
//    DiversityEvaluator::operator()(Turingforge::RandomGenerator& random, Individual& ind, Turingforge::Span<Turingforge::Scalar>  /*buf*/) const -> typename EvaluatorBase::ReturnType
//    {
//        (void)ind.Genotype.Hash(hashmode_);
//        std::vector<Turingforge::Hash> lhs(ind.Genotype.Length());
//        auto const& nodes = ind.Genotype.Nodes();
//        std::transform(nodes.begin(), nodes.end(), lhs.begin(), [](auto const& n) { return n.CalculatedHashValue; });
//        std::stable_sort(lhs.begin(), lhs.end());
//        auto const& values = divmap_.values();
//
//        Turingforge::Scalar distance{0};
//        std::vector<double> distances(sampleSize_);
//        for (auto i = 0UL; i < sampleSize_; ++i) {
//            auto const& rhs = Turingforge::Random::Sample(random, values.begin(), values.end())->second;
//            distance += static_cast<Turingforge::Scalar>(Turingforge::Distance::Jaccard(lhs, rhs));
//        }
//        return EvaluatorBase::ReturnType { -distance / static_cast<Turingforge::Scalar>(sampleSize_) };
//    }
//
//    auto
//    AggregateEvaluator::operator()(Turingforge::RandomGenerator& rng, Individual& ind, Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType
//    {
//        using vstat::univariate::accumulate;
//        auto f = evaluator_.get()(rng, ind, buf);
//        switch(aggtype_) {
//            case AggregateType::Min: {
//                return { *std::min_element(f.begin(), f.end()) };
//            }
//            case AggregateType::Max: {
//                return { *std::max_element(f.begin(), f.end()) };
//            }
//            case AggregateType::Median: {
//                auto const sz { std::ssize(f) };
//                auto const a = f.begin() + sz / 2;
//                std::nth_element(f.begin(), a, f.end());
//                if (sz % 2 == 0) {
//                    auto const b = std::max_element(f.begin(), a);
//                    return { (*a + *b) / 2 };
//                }
//                return { *a };
//            }
//            case AggregateType::Mean: {
//                return { static_cast<Turingforge::Scalar>(accumulate<Turingforge::Scalar>(f.begin(), f.end()).mean) };
//            }
//            case AggregateType::HarmonicMean: {
//                auto stats = accumulate<Turingforge::Scalar>(f.begin(), f.end(), [](auto x) { return 1/x; });
//                return { static_cast<Turingforge::Scalar>(stats.count / stats.sum) };
//            }
//            case AggregateType::Sum: {
//                return { static_cast<Turingforge::Scalar>(vstat::univariate::accumulate<Turingforge::Scalar>(f.begin(), f.end()).sum) };
//            }
//            default: {
//                throw std::runtime_error("Unknown AggregateType");
//            }
//        }
//    }
//
//    template<> auto OPERON_EXPORT
//    MinimumDescriptionLengthEvaluator<DefaultDispatch>::operator()(Turingforge::RandomGenerator& rng, Individual& ind, Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType {
//        auto const& problem = Evaluator::GetProblem();
//        auto const range = problem.TrainingRange();
//        auto const& dataset = problem.GetDataset();
//        auto const& nodes = ind.Genotype.Nodes();
//        auto const& dtable = Evaluator::GetDispatchTable();
//
//        auto const* optimizer = Evaluator::GetOptimizer();
//        EXPECT(optimizer != nullptr);
//
//        // this call will optimize the tree coefficients and compute the SSE
//        auto const& tree = ind.Genotype;
//        Turingforge::Interpreter<Turingforge::Scalar, DefaultDispatch> interpreter{dtable, dataset, ind.Genotype};
//        auto summary = optimizer->Optimize(rng, tree);
//        auto parameters = summary.Success ? summary.FinalParameters : tree.GetCoefficients();
//        auto const p { static_cast<double>(parameters.size()) };
//
//        std::vector<Turingforge::Scalar> buffer;
//        if (buf.size() < range.Size()) {
//            buffer.resize(range.Size());
//            buf = Turingforge::Span<Turingforge::Scalar>(buffer);
//        }
//        interpreter.Evaluate(parameters, range, buf);
//
//        auto estimatedValues = buf;
//        auto targetValues    = problem.TargetValues(range);
//
//        // codelength of the complexity
//        // count number of unique functions
//        // - count weight * variable as three nodes
//        // - compute complexity c of the remaining numerical values
//        //   (that are not part of the coefficients that are optimized)
//        Turingforge::Set<Turingforge::Hash> uniqueFunctions; // to count the number of unique functions
//        auto k{0.0}; // number of nodes
//        auto cComplexity { 0.0 };
//
//        // codelength of the parameters
//        Eigen::Matrix<Turingforge::Scalar, -1, -1> j = interpreter.JacRev(parameters, range); // jacobian
//        auto fm = optimizer->ComputeFisherMatrix(estimatedValues, {j.data(), static_cast<std::size_t>(j.size())}, sigma_);
//        auto ii = fm.diagonal().array();
//        ENSURE(ii.size() == p);
//
//        auto cParameters { 0.0 };
//        auto constexpr eps = std::numeric_limits<Turingforge::Scalar>::epsilon(); // machine epsilon for zero comparison
//
//        for (auto i = 0, j = 0; i < std::ssize(nodes); ++i) {
//            auto const& n = nodes[i];
//
//            // count the number of nodes and the number of unique operators
//            k += n.IsVariable() ? 3 : 1;
//            uniqueFunctions.insert(n.HashValue);
//
//            if (n.Optimize) {
//                // this branch computes the description length of the parameters to be optimized
//                auto const di = std::sqrt(12 / ii(j));
//                auto const ci = std::abs(parameters[j]);
//
//                if (!(std::isfinite(ci) && std::isfinite(di)) || ci / di < 1) {
//                    //ind.Genotype[i].Optimize = false;
//                    //auto const v = ind.Genotype[i].Value;
//                    //ind.Genotype[i].Value = 0;
//                    //auto fit = (*this)(rng, ind, buf);
//                    //ind.Genotype[i].Optimize = true;
//                    //ind.Genotype[i].Value = v;
//                    //return fit;
//                } else {
//                    cParameters += 0.5 * std::log(ii(j)) + std::log(ci);
//                }
//                ++j;
//            } else {
//                // this branch computes the description length of the remaining tree structure
//                if (std::abs(n.Value) < eps) { continue; }
//                cComplexity += std::log(std::abs(n.Value));
//            }
//        }
//
//        auto q { static_cast<double>(uniqueFunctions.size()) };
//        if (q > 0) { cComplexity += static_cast<double>(k) * std::log(q); }
//
//        cParameters -= p/2 * std::log(3);
//
//        auto cLikelihood = optimizer->ComputeLikelihood(estimatedValues, targetValues, sigma_);
//        auto mdl = cComplexity + cParameters + cLikelihood;
//        if (!std::isfinite(mdl)) { mdl = EvaluatorBase::ErrMax; }
//        return typename EvaluatorBase::ReturnType { static_cast<Turingforge::Scalar>(mdl) };
//    }
//
//    template<> auto OPERON_EXPORT
//    BayesianInformationCriterionEvaluator<DefaultDispatch>::operator()(Turingforge::RandomGenerator& rng, Individual& ind, Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType {
//        auto const& tree = ind.Genotype;
//        auto p = static_cast<Turingforge::Scalar>(std::ranges::count_if(tree.Nodes(), &Turingforge::Function::Optimize));
//        auto n = static_cast<Turingforge::Scalar>(Evaluator::GetProblem().TrainingRange().Size());
//        auto mse = Evaluator::operator()(rng, ind, buf).front();
//        auto bic = n * std::log(mse) + p * std::log(n);
//        if (!std::isfinite(bic)) { bic = EvaluatorBase::ErrMax; }
//        return typename EvaluatorBase::ReturnType { static_cast<Turingforge::Scalar>(bic) };
//    }
//
//    template<> auto OPERON_EXPORT
//    AkaikeInformationCriterionEvaluator<DefaultDispatch>::operator()(Turingforge::RandomGenerator& rng, Individual& ind, Turingforge::Span<Turingforge::Scalar> buf) const -> typename EvaluatorBase::ReturnType {
//        auto mse = Evaluator::operator()(rng, ind, buf).front();
//        auto n = static_cast<Turingforge::Scalar>(Evaluator::GetProblem().TrainingRange().Size());
//        auto aik = n/2 * (std::log(Turingforge::Math::Tau) + std::log(mse) + 1);
//        if (!std::isfinite(aik)) { aik = EvaluatorBase::ErrMax; }
//        return typename EvaluatorBase::ReturnType { static_cast<Turingforge::Scalar>(aik) };
//    }
//} // namespace Turingforge

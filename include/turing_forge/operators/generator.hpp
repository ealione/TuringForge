#pragma once

#include "turing_forge/core/operator.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/operators/evaluator.hpp"
#include "turing_forge/operators/mutation.hpp"
#include "turing_forge/operators/selector.hpp"
#include "turing_forge/operators/local_search.hpp"
#include "turing_forge/optimizer/optimizer.hpp"

namespace Turingforge {

    struct RecombinationResult {
        std::optional<Turingforge::Individual> Child;
        std::optional<Turingforge::Individual> Parent1;
        std::optional<Turingforge::Individual> Parent2;

        explicit operator bool() const { return Child.has_value(); }
    };

    class OffspringGeneratorBase
            : public OperatorBase<std::optional<Individual>, /* crossover prob. */ double, /* mutation prob. */ double, /* local search prob. */ double, /* memory buffer */ Turingforge::Span<Turingforge::Scalar>> {
    public:
        OffspringGeneratorBase(EvaluatorBase &eval, CrossoverBase &cx, MutatorBase &mut, SelectorBase &femSel, SelectorBase &maleSel, CoefficientOptimizer const *coeffOptimizer = nullptr)
                : evaluator_(eval), crossover_(cx), mutator_(mut), femaleSelector_(femSel), maleSelector_(maleSel), coeffOptimizer_{coeffOptimizer} {
        }

        [[nodiscard]] auto FemaleSelector() const -> SelectorBase & { return femaleSelector_.get(); }

        [[nodiscard]] auto MaleSelector() const -> SelectorBase & { return maleSelector_.get(); }

        [[nodiscard]] auto Crossover() const -> CrossoverBase & { return crossover_.get(); }

        [[nodiscard]] auto Mutator() const -> MutatorBase & { return mutator_.get(); }

        [[nodiscard]] auto Evaluator() const -> EvaluatorBase & { return evaluator_.get(); }

        [[nodiscard]] auto Optimizer() const -> CoefficientOptimizer const * { return coeffOptimizer_; }

        virtual auto Prepare(Turingforge::Span<Individual const> pop) const -> void {
            this->FemaleSelector().Prepare(pop);
            this->MaleSelector().Prepare(pop);
            this->Evaluator().Prepare(pop);
        }

        [[nodiscard]] virtual auto Terminate() const -> bool { return evaluator_.get().BudgetExhausted(); }

        auto Generate(Turingforge::RandomGenerator &random, double pCrossover, double pMutation, double pLocal,
                      Turingforge::Span<Turingforge::Scalar> buf, RecombinationResult &res) const -> void {
            auto pop = FemaleSelector().Population();
            if (!res.Parent1) {
                res.Parent1 = pop[FemaleSelector()(random)];
            }

            // Crossover
            using BernoulliTrial = std::bernoulli_distribution;
            if (BernoulliTrial{pCrossover}(random)) {
                if (!res.Parent2) {
                    res.Parent2 = pop[MaleSelector()(random)];
                }
                res.Child = Crossover()(random, res.Parent1.value(), res.Parent2.value());
            }

            // Mutation
            if (BernoulliTrial{pMutation}(random)) {
                if (res.Child) {
                    res.Child = Mutator()(random, std::move(res.Child.value()));
                } else {
                    res.Child = Mutator()(random, res.Parent1.value());
                }
            }

            if (!res.Child) res.Child = res.Parent1;

            // Local optimization
            if (coeffOptimizer_ != nullptr && BernoulliTrial{pLocal}(random)) {
                auto summary = (*coeffOptimizer_)(random, res.Child.value());
                Evaluator().ResidualEvaluations += summary.FunctionEvaluations;
                Evaluator().JacobianEvaluations += summary.JacobianEvaluations;
            }

            res.Child->Fitness = Evaluator()(random, res.Child.value(), buf);
            for (auto &v: res.Child->Fitness) {
                if (!std::isfinite(v)) { v = std::numeric_limits<Turingforge::Scalar>::max(); }
            }
        }

        auto Generate(Turingforge::RandomGenerator &random, double pCrossover, double pMutation, double pLocal,
                      Turingforge::Span<Turingforge::Scalar> buf) const -> RecombinationResult {
            RecombinationResult res;
            Generate(random, pCrossover, pMutation, pLocal, buf, res);
            return res;
        }

    private:
        std::reference_wrapper<EvaluatorBase> evaluator_;
        std::reference_wrapper<CrossoverBase> crossover_;
        std::reference_wrapper<MutatorBase> mutator_;
        std::reference_wrapper<SelectorBase> femaleSelector_;
        std::reference_wrapper<SelectorBase> maleSelector_;
        CoefficientOptimizer const *coeffOptimizer_;
    };

    class BasicOffspringGenerator final : public OffspringGeneratorBase {
    public:
        explicit BasicOffspringGenerator(EvaluatorBase &eval, CrossoverBase &cx, MutatorBase &mut, SelectorBase &femSel,
                                         SelectorBase &maleSel, CoefficientOptimizer const *coeffOptimizer = nullptr)
                : OffspringGeneratorBase(eval, cx, mut, femSel, maleSel, coeffOptimizer) {
        }

        auto operator()(Turingforge::RandomGenerator &random, double pCrossover, double pMutation, double pLocal,
                        Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual> final;
    };

    class BroodOffspringGenerator : public OffspringGeneratorBase {
    public:
        explicit BroodOffspringGenerator(EvaluatorBase &eval, CrossoverBase &cx, MutatorBase &mut, SelectorBase &femSel,
                                         SelectorBase &maleSel, CoefficientOptimizer const *coeffOptimizer = nullptr)
                : OffspringGeneratorBase(eval, cx, mut, femSel, maleSel, coeffOptimizer), broodSize_(DefaultBroodSize) {
        }

        auto operator()(Turingforge::RandomGenerator &random, double pCrossover, double pMutation, double pLocal,
                        Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual> final;

        void BroodSize(size_t value) { broodSize_ = value; }

        [[nodiscard]] auto BroodSize() const -> size_t { return broodSize_; }

        static constexpr size_t DefaultBroodSize{10};

    private:
        size_t broodSize_;
    };

    class PolygenicOffspringGenerator : public OffspringGeneratorBase {
    public:
        explicit PolygenicOffspringGenerator(EvaluatorBase &eval, CrossoverBase &cx, MutatorBase &mut,
                                             SelectorBase &femSel, SelectorBase &maleSel,
                                             CoefficientOptimizer const *coeffOptimizer = nullptr)
                : OffspringGeneratorBase(eval, cx, mut, femSel, maleSel, coeffOptimizer), broodSize_(DefaultBroodSize) {
        }

        auto operator()(Turingforge::RandomGenerator &random, double pCrossover, double pMutation, double pLocal,
                        Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual> final;

        void PolygenicSize(size_t value) { broodSize_ = value; }

        [[nodiscard]] auto PolygenicSize() const -> size_t { return broodSize_; }

        static constexpr size_t DefaultBroodSize = 10;

    private:
        size_t broodSize_;
    };

    class OffspringSelectionGenerator : public OffspringGeneratorBase {
    public:
        explicit OffspringSelectionGenerator(EvaluatorBase &eval, CrossoverBase &cx, MutatorBase &mut,
                                             SelectorBase &femSel, SelectorBase &maleSel,
                                             CoefficientOptimizer const *coeffOptimizer = nullptr)
                : OffspringGeneratorBase(eval, cx, mut, femSel, maleSel, coeffOptimizer) {
        }

        auto operator()(Turingforge::RandomGenerator &random, double pCrossover, double pMutation, double pLocal,
                        Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual> final;

        void MaxSelectionPressure(size_t value) { maxSelectionPressure_ = value; }

        auto MaxSelectionPressure() const -> size_t { return maxSelectionPressure_; }

        void ComparisonFactor(double value) { comparisonFactor_ = value; }

        auto ComparisonFactor() const -> double { return comparisonFactor_; }

        void Prepare(const Turingforge::Span<const Individual> pop) const override {
            OffspringGeneratorBase::Prepare(pop);
            lastEvaluations_ = this->Evaluator().TotalEvaluations();
        }

        auto SelectionPressure() const -> double {
            auto n = this->FemaleSelector().Population().size();
            if (n == 0U) {
                return 0;
            }
            auto e = this->Evaluator().TotalEvaluations() - lastEvaluations_;
            return static_cast<double>(e) / static_cast<double>(n);
        }

        auto Terminate() const -> bool override {
            return OffspringGeneratorBase::Terminate() ||
                   SelectionPressure() > static_cast<double>(maxSelectionPressure_);
        };

        static constexpr size_t DefaultMaxSelectionPressure{100};
        static constexpr double DefaultComparisonFactor{1.0};

    private:
        mutable size_t lastEvaluations_{0};
        size_t maxSelectionPressure_{DefaultMaxSelectionPressure};
        double comparisonFactor_{0};
    };

} // namespace Turingforge
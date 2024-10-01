#pragma once

#include <functional>                      // for reference_wrapper, function
#include <thread>                          // for thread
#include <utility>                         // for move
#include <vector>                          // for vector

#include "turing_forge/algorithms/config.hpp"    // for GeneticAlgorithmConfig
#include "turing_forge/algorithms/ga_base.hpp"
#include "turing_forge/core/individual.hpp"      // for Individual
#include "turing_forge/core/types.hpp"           // for Span, Vector, RandomGenerator
#include "turing_forge/operators/evaluator.hpp"  // for EvaluatorBase
#include "turing_forge/operators/generator.hpp"  // for OffspringGeneratorBase

// forward declaration
namespace tf { class Executor; }

namespace Turingforge {

class NondominatedSorterBase;
class Problem;
class ReinserterBase;
struct CoefficientInitializerBase;
struct IndividualInitializerBase;

class NSGA2 : public GeneticAlgorithmBase {
    std::reference_wrapper<const NondominatedSorterBase> sorter_;
    std::vector<std::vector<size_t>> fronts_;
    Turingforge::Vector<Individual> best_; // best Pareto front

    auto UpdateDistance(Turingforge::Span<Individual> pop) -> void;
    auto Sort(Turingforge::Span<Individual> pop) -> void;

public:
    NSGA2(Problem const& problem, AlgorithmConfig const& config, IndividualInitializerBase const& individualInit, CoefficientInitializerBase const& coeffInit, OffspringGeneratorBase const& generator, ReinserterBase const& reinserter, NondominatedSorterBase const& sorter)
        : GeneticAlgorithmBase(problem, config, individualInit, coeffInit, generator, reinserter), sorter_(sorter)
    {
        auto const nobj { GetGenerator().Evaluator().ObjectiveCount() };
        for (auto& ind : Individuals()) {
            ind.Fitness.resize(nobj, EvaluatorBase::ErrMax);
        }
    }

    [[nodiscard]] auto Best() const -> Turingforge::Span<Individual const> { return { best_.data(), best_.size() }; }

    auto Run(tf::Executor& /*executor*/, Turingforge::RandomGenerator&/*rng*/, std::function<void()> /*report*/ = nullptr) -> void;
    auto Run(Turingforge::RandomGenerator& /*rng*/, std::function<void()> /*report*/ = nullptr, size_t /*threads*/= 0) -> void;
};
} // namespace Turingforge
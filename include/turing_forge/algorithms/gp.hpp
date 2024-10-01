#pragma once

#include <cstddef>                         // for size_t
#include <functional>                      // for reference_wrapper, function
#include <thread>                          // for thread
#include <utility>                         // for move

#include "turing_forge/algorithms/config.hpp"    // for AlgorithmConfig
#include "turing_forge/algorithms/ga_base.hpp"
#include "turing_forge/core/individual.hpp"      // for Individual
#include "turing_forge/core/types.hpp"           // for Span, Vector, RandomGenerator
#include "turing_forge/operators/evaluator.hpp"  // for EvaluatorBase
#include "turing_forge/operators/generator.hpp"  // for OffspringGeneratorBase

// forward declaration
namespace tf { class Executor; }

namespace Turingforge {

    class Problem;
    class ReinserterBase;
    struct CoefficientInitializerBase;
    struct IndividualInitializerBase;

    class GeneticProgrammingAlgorithm : public GeneticAlgorithmBase {
    public:
        GeneticProgrammingAlgorithm(Problem const& problem, AlgorithmConfig const& config, IndividualInitializerBase const& individualInit, CoefficientInitializerBase const& coeffInit, OffspringGeneratorBase const& generator, ReinserterBase const& reinserter)
            : GeneticAlgorithmBase(problem, config, individualInit, coeffInit, generator, reinserter)
        {
        }

        auto Run(tf::Executor& /*executor*/, Turingforge::RandomGenerator&/*rng*/, std::function<void()> /*report*/ = nullptr) -> void;
        auto Run(Turingforge::RandomGenerator& /*rng*/, std::function<void()> /*report*/ = nullptr, size_t /*threads*/= 0) -> void;
    };
} // namespace Turingforge
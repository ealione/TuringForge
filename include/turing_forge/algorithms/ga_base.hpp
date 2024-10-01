#pragma once

#include <functional>

#include "turing_forge/operators/generator.hpp"
#include "config.hpp"

namespace Turingforge {

class Problem;
class ReinserterBase;
struct CoefficientInitializerBase;
struct IndividualInitializerBase;

class GeneticAlgorithmBase {
public:
    virtual ~GeneticAlgorithmBase() = default;
    GeneticAlgorithmBase(const GeneticAlgorithmBase&) = default;
    GeneticAlgorithmBase(GeneticAlgorithmBase&&) = delete;
    auto operator=(const GeneticAlgorithmBase&) -> GeneticAlgorithmBase& = default;
    auto operator=(GeneticAlgorithmBase&&) -> GeneticAlgorithmBase& = delete;

    GeneticAlgorithmBase(Problem const& problem, AlgorithmConfig const& config, IndividualInitializerBase const& individualInit, CoefficientInitializerBase const& coeffInit, OffspringGeneratorBase const& generator, ReinserterBase const& reinserter)
        : problem_(problem)
        , config_(config)
        , individualInit_(individualInit)
        , coeffInit_(coeffInit)
        , generator_(generator)
        , reinserter_(reinserter)
        , individuals_(config.PopulationSize + config.PoolSize)
        , parents_(individuals_.data(), config.PopulationSize)
        , offspring_(individuals_.data() + config.PopulationSize, config.PoolSize)
    {
    }

    [[nodiscard]] auto Parents() const -> Turingforge::Span<Individual const> { return { parents_.data(), parents_.size() }; }
    auto Parents() -> Turingforge::Span<Individual> { return parents_; }

    [[nodiscard]] auto Offspring() const -> Turingforge::Span<Individual const> { return { offspring_.data(), offspring_.size() }; }
    auto Offspring() -> Turingforge::Span<Individual> { return offspring_; }

    [[nodiscard]] auto Individuals() -> Turingforge::Vector<Turingforge::Individual>& { return individuals_; }
    [[nodiscard]] auto Individuals() const -> Turingforge::Vector<Turingforge::Individual> const& { return individuals_; }

    [[nodiscard]] auto GetProblem() const -> const Problem& { return problem_.get(); }
    [[nodiscard]] auto GetConfig() const -> const AlgorithmConfig& { return config_.get(); }

    [[nodiscard]] auto GetIndividualInitializer() const -> IndividualInitializerBase const& { return individualInit_.get(); }
    [[nodiscard]] auto GetCoefficientInitializer() const -> CoefficientInitializerBase const& { return coeffInit_.get(); }
    [[nodiscard]] auto GetGenerator() const -> const OffspringGeneratorBase& { return generator_.get(); }
    [[nodiscard]] auto GetReinserter() const -> const ReinserterBase& { return reinserter_.get(); }

    [[nodiscard]] auto Generation() const -> size_t { return generation_; }
    auto Generation() -> size_t& { return generation_; }

    auto Reset() -> void
    {
        generation_ = 0;
        GetGenerator().Evaluator().Reset();
    }

private:
    std::reference_wrapper<const Problem> problem_;
    std::reference_wrapper<const AlgorithmConfig> config_;
    std::reference_wrapper<const IndividualInitializerBase> individualInit_;
    std::reference_wrapper<const CoefficientInitializerBase> coeffInit_;
    std::reference_wrapper<const OffspringGeneratorBase> generator_;
    std::reference_wrapper<const ReinserterBase> reinserter_;

    Turingforge::Vector<Individual> individuals_;
    Turingforge::Span<Individual> parents_;
    Turingforge::Span<Individual> offspring_;

    size_t generation_{0};
};

} // namespace Turingforge
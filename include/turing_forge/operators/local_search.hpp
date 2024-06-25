#pragma once

#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/operator.hpp"

namespace Turingforge {

    // forward declarations
    class OptimizerBase;
    struct OptimizerSummary;

    class CoefficientOptimizer : public OperatorBase<OptimizerSummary, Turingforge::Individual&> {
    public:
    explicit CoefficientOptimizer(OptimizerBase const& optimizer, double lmProb = 1.0)
    : optimizer_(optimizer)
    , lamarckianProbability_(lmProb)
    { }

    // convenience
    auto operator()(Turingforge::RandomGenerator& rng, Turingforge::Individual& individual) const -> OptimizerSummary override;

    private:

    std::reference_wrapper<Turingforge::OptimizerBase const> optimizer_;
    double lamarckianProbability_{1.0};
};

} // namespace Turingforge
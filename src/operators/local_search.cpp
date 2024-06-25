#include "turing_forge/operators//local_search.hpp"
#include "turing_forge/core/individual.hpp"
#include "turing_forge/optimizer/optimizer.hpp"

namespace Turingforge {

    auto CoefficientOptimizer::operator()(Turingforge::RandomGenerator& rng, Turingforge::Individual& individual) const -> OptimizerSummary {
        OptimizerSummary summary;
        auto const& optimizer = optimizer_.get();
        if (optimizer.Iterations() > 0) {
            summary = optimizer.Optimize(rng, individual);

            if (std::bernoulli_distribution(lamarckianProbability_)(rng) && summary.Success) {
                individual.Coefficients = summary.FinalParameters;
            }
        }
        return summary;
    }
} // namespace Turingforge
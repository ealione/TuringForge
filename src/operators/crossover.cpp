#include <fmt/core.h>
#include <random>

#include "turing_forge/core/contracts.hpp"
#include "turing_forge/operators/crossover.hpp"
#include "turing_forge/random/random.hpp"

namespace Turingforge {

    namespace {
        using Limits = std::pair<size_t, size_t>;

        auto NotIn(Limits t, size_t v) -> bool {
            auto [a, b] = t;
            return v < a || b < v;
        }
    } // namespace

    static auto SelectRandomInteraction(Turingforge::RandomGenerator& random, Individual const& individual, double internalProb, Limits length) -> size_t
    {
        if (individual.Length == 1 || NotIn(length, individual.Length)) {
            return 0;
        }

        std::vector<size_t> candidates(individual.Length);
        auto head = candidates.begin();
        auto tail = candidates.rbegin();

        // check if we have any function node candidates at all and if the bernoulli trial succeeds
        if (tail > candidates.rbegin() && std::bernoulli_distribution(internalProb)(random)) {
            return *Turingforge::Random::Sample(random, candidates.rbegin(), tail);
        }
        return *Turingforge::Random::Sample(random, candidates.begin(), head);
    }

    auto IndividualCrossover::FindCompatibleSwapLocations(Turingforge::RandomGenerator& random, Individual const& lhs, Individual const& rhs) const -> std::pair<size_t, size_t>
    {
        using Signed = std::make_signed<size_t>::type;
        auto diff = static_cast<Signed>(lhs.Length - maxLength_ + 1); // +1 to account for at least one node that gets swapped in

        auto i = SelectRandomInteraction(random, lhs, internalProbability_, Limits{std::max(diff, Signed{1}), lhs.Length});

        auto partialTreeLength = lhs.Length - 1;
        auto maxBranchLength = static_cast<Signed>(maxLength_ - partialTreeLength);
        maxBranchLength = std::max(maxBranchLength, Signed{1});

        auto j = SelectRandomInteraction(random, rhs, internalProbability_, Limits{1UL, maxBranchLength});
        return std::make_pair(i, j);
    }

    auto IndividualCrossover::operator()(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> Individual
    {
        auto [i, j] = FindCompatibleSwapLocations(random, lhs, rhs);
        auto child = Cross(lhs, rhs, i, j);

        // TODO: `maxLength_` type?
        auto maxLength{std::max(static_cast<uint16_t>(maxLength_), lhs.Length)};

        ENSURE(child.Length <= maxLength);

        return child;
    }
} // namespace Turingforge
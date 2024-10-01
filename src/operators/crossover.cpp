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

    static auto SelectRandomInteraction(Turingforge::RandomGenerator& random, size_t length, double internalProb, Limits limits) -> size_t
    {
        if (length <= 1 || NotIn(limits, length)) {
            return 0;
        }

        size_t start = std::min(limits.first, length - 1);
        size_t end = std::min(limits.second, length);

        if (std::bernoulli_distribution(internalProb)(random)) {
            start = (start + end) / 2;  // Select from the second half
        }

        std::uniform_int_distribution<size_t> dist(start, end - 1);
        return dist(random);
    }

    auto IndividualCrossover::FindCompatibleSwapLocations(Turingforge::RandomGenerator& random, size_t lhsLength, size_t rhsLength) const -> std::pair<size_t, size_t>
    {
        using Signed = std::make_signed<size_t>::type;

        Signed iLower = std::max(Signed{1}, static_cast<Signed>(lhsLength) - static_cast<Signed>(maxLength_) + 1);
        size_t i = SelectRandomInteraction(random, lhsLength, internalProbability_, {static_cast<size_t>(iLower), lhsLength});

        Signed jUpper = std::max(Signed{1}, static_cast<Signed>(maxLength_) - static_cast<Signed>(lhsLength) + static_cast<Signed>(i) + 1);
        size_t j = SelectRandomInteraction(random, rhsLength, internalProbability_, {1, static_cast<size_t>(jUpper)});

        return {i, j};
    }

    auto IndividualCrossover::operator()(Turingforge::RandomGenerator& random, const Individual& lhs, const Individual& rhs) const -> Individual
    {
        auto [i, j] = FindCompatibleSwapLocations(random, lhs.Length, rhs.Length);
        auto child = Cross(lhs, rhs, i, j);

        auto maxLength{std::max(static_cast<uint16_t>(maxLength_), lhs.Length)};

        ENSURE(child.Length <= maxLength);

        return child;
    }
} // namespace Turingforge
#include <cstddef>
#include <numeric>
#include <vector>

#include "turing_forge/operators/selector.hpp"
#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/types.hpp"

namespace Turingforge {

    auto TournamentSelector::operator()(Turingforge::RandomGenerator& random) const -> size_t
    {
        auto population = Population();
        std::uniform_int_distribution<size_t> uniformInt(0, population.size() - 1);
        auto best = uniformInt(random);
        auto tournamentSize = GetTournamentSize();

        for (size_t i = 1; i < tournamentSize; ++i) {
            auto curr = uniformInt(random);
            if (this->Compare(population[curr], population[best])) {
                best = curr;
            }
        }
        return best;
    }

    auto RankTournamentSelector::operator()(Turingforge::RandomGenerator& random) const -> size_t
    {
        auto population = Population();
        std::uniform_int_distribution<size_t> uniformInt(0, population.size() - 1);
        auto best = uniformInt(random);
        auto tournamentSize = GetTournamentSize();

        for (size_t i = 1; i < tournamentSize; ++i) {
            auto curr = uniformInt(random);
            if (best < curr) {
                best = curr;
            }
        }
        return best;
    }

    void RankTournamentSelector::Prepare(const Turingforge::Span<const Individual> pop) const
    {
        SelectorBase::Prepare(pop);
        indices_.resize(pop.size());
        std::iota(indices_.begin(), indices_.end(), 0);
        std::sort(indices_.begin(), indices_.end(), [&](auto i, auto j) { return Compare(pop[i], pop[j]); });
    }
} // namespace Turingforge
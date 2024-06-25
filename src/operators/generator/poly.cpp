#include "turing_forge/operators/generator.hpp"
#include "turing_forge/operators/non_dominated_sorter.hpp"

namespace Turingforge {
    auto PolygenicOffspringGenerator::operator()(Turingforge::RandomGenerator& random, double pCrossover, double pMutation, double pLocal, Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual>
    {
        std::uniform_real_distribution<double> uniformReal;
        auto pop = this->FemaleSelector().Population();

        // assuming the basic generator never fails
        auto makeOffspring = [&]() {
            auto res = OffspringGeneratorBase::Generate(random, pCrossover, pMutation, pLocal, buf);
            return res ? res.Child.value() : res.Parent1.value();
        };

        std::vector<Individual> offspring(broodSize_);
        std::generate(offspring.begin(), offspring.end(), makeOffspring);
        SingleObjectiveComparison comp{0};

        if (pop.front().Size() > 1) {
            std::stable_sort(offspring.begin(), offspring.end(), LexicographicalComparison{});
            auto fronts = RankIntersectSorter{}(offspring);
            auto best = *std::min_element(fronts[0].begin(), fronts[0].end(), [&](auto i, auto j) { return comp(offspring[i], offspring[j]); });
            return std::make_optional(offspring[best]);
        }
        auto best = *std::min_element(offspring.begin(), offspring.end(), comp);
        return std::make_optional(best);
    }

} // namespace Turingforge
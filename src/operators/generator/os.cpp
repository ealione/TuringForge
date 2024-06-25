#include "turing_forge/operators/generator.hpp"
#include "turing_forge/core/comparison.hpp"

namespace Turingforge {

    auto OffspringSelectionGenerator::operator()(Turingforge::RandomGenerator& random, double pCrossover, double pMutation, double pLocal, Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual>
    {
        auto res = OffspringGeneratorBase::Generate(random, pCrossover, pMutation, pLocal, buf);
        bool accept{false};
        if (res.Parent2) {
            Individual q{};
            for (size_t i = 0; i < q.Size(); ++i) {
                auto f1 = (*res.Parent1)[i];
                auto f2 = (*res.Parent2)[i];
                q[i] = std::max(f1, f2) - static_cast<Turingforge::Scalar>(comparisonFactor_) * std::abs(f1 - f2);
                accept = Turingforge::ParetoDominance{}(res.Child->Fitness, q.Fitness) != Dominance::Right;
            }
        } else {
            accept = Turingforge::ParetoDominance{}(res.Child->Fitness, res.Parent1->Fitness) != Dominance::Right;
        }
        return accept ? res.Child : std::nullopt;
    }

} // namespace Turingforge
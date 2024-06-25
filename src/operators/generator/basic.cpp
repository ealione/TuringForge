#include "turing_forge/operators/generator.hpp"

namespace Turingforge {
    auto BasicOffspringGenerator::operator()(Turingforge::RandomGenerator& random, double pCrossover, double pMutation, double pLocal, Turingforge::Span<Turingforge::Scalar> buf) const -> std::optional<Individual>
    {
        auto res = OffspringGeneratorBase::Generate(random, pCrossover, pMutation, pLocal, buf);
        return res.Child;
    }
} // namespace Turingforge
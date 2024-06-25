#pragma once

#include <utility>

#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/variable.hpp"

namespace Turingforge {

class Individual;
class PrimitiveSet;

// the creator builds a new tree using the existing pset and allowed inputs
struct CreatorBase : public OperatorBase<Individual, size_t, Turingforge::Scalar, size_t> {
    CreatorBase(PrimitiveSet const& pset, std::vector<Turingforge::Hash> variables)
        : pset_(pset)
        , variables_(std::move(variables))
    {
    }

    [[nodiscard]] auto GetPrimitiveSet() const -> PrimitiveSet const& { return pset_.get(); }
    void SetPrimitiveSet(PrimitiveSet const& pset) { pset_ = pset; }

    [[nodiscard]] auto GetVariables() const -> Turingforge::Span<Turingforge::Hash const> { return variables_; }
    auto SetVariables(Turingforge::Span<Turingforge::Hash const> variables) { variables_ = std::vector<Turingforge::Hash>(variables.begin(), variables.end()); }

private:
    std::reference_wrapper<PrimitiveSet const> pset_;
    std::vector<Turingforge::Hash> variables_;
};

class BalancedCreator final : public CreatorBase {
public:
    BalancedCreator(PrimitiveSet const& pset, std::vector<Turingforge::Hash> variables, double bias = 0.0)
        : CreatorBase(pset, std::move(variables))
        , irregularityBias_(bias)
    {
    }

    auto operator()(Turingforge::RandomGenerator& func, size_t terms, Turingforge::Scalar exponent_limit, size_t objectives) const -> Individual override;

    void SetBias(double bias) { irregularityBias_ = bias; }
    [[nodiscard]] auto GetBias() const -> double { return irregularityBias_; }

private:
    double irregularityBias_;
};

} // namespace Turingforge
#pragma once

#include "creator.hpp"
#include "turing_forge/core/individual.hpp"

namespace Turingforge {
struct CoefficientInitializerBase : public OperatorBase<void, Individual&> {
};

template <typename Dist>
struct CoefficientInitializer : public CoefficientInitializerBase {
    using NodeCheckCallback = std::function<bool(Turingforge::Function)>;

    explicit CoefficientInitializer(NodeCheckCallback callback)
        : callback_(std::move(callback))
    {
    }

    CoefficientInitializer()
        : CoefficientInitializer([](auto const& node) { return node.IsLeaf(); })
    {
    }

    auto operator()(Turingforge::RandomGenerator& random, Turingforge::Individual& individual) const -> void override
    {
        for (auto& node : individual.Nodes()) {
            if (callback_(node)) {
                node.Value = Dist(params_)(random);
            }
        }
    }

    template <typename... Args>
    auto ParameterizeDistribution(Args... args) const -> void
    {
        params_ = typename Dist::param_type { std::forward<Args&&>(args)... };
    }

private:
    mutable typename Dist::param_type params_;
    NodeCheckCallback callback_;
};

// wraps a creator and generates trees from a given size distribution
using UniformCoefficientInitializer = CoefficientInitializer<std::uniform_real_distribution<Turingforge::Scalar>>;
using NormalCoefficientInitializer = CoefficientInitializer<std::normal_distribution<Turingforge::Scalar>>;

} // namespace Turingforge


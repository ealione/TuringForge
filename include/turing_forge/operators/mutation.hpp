#pragma once

#include <utility>

#include "turing_forge/core/operator.hpp"
#include "turing_forge/core/variable.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/core/individual.hpp"

namespace Turingforge {

    struct CoefficientInitializerBase;
    struct CreatorBase;

// the mutator can work in place or return a copy (child)
    struct MutatorBase : public OperatorBase<Individual, Individual> {
    };

    template<typename Dist>
    struct OnePointMutation : public MutatorBase {
        auto operator()(Turingforge::RandomGenerator &random, Individual individual) const -> Individual override {
            auto &coefficients = individual.Coefficients;
            // sample a random item
            // TODO: remove the `!=0` condition
            auto it = Turingforge::Random::Sample(random, coefficients.begin(), coefficients.end(),
                                                  [](auto const &n) { return n != 0; });
            EXPECT(it < coefficients.end());
            it += Dist(params_)(random);

            return individual;
        }

        template<typename... Args>
        auto ParameterizeDistribution(Args... args) const -> void {
            params_ = typename Dist::param_type{std::forward<Args &&>(args)...};
        }

    private:
        mutable typename Dist::param_type params_;
    };

    template<typename Dist>
    struct MultiPointMutation : public MutatorBase {
        auto operator()(Turingforge::RandomGenerator &random, Individual individual) const -> Individual override {
            for (auto &coefficient: individual.Coefficients) {
                // TODO: should this be `+=`?
                coefficient += Dist(params_)(random);
            }
            return individual;
        }

        template<typename... Args>
        auto ParameterizeDistribution(Args... args) const -> void {
            params_ = typename Dist::param_type{std::forward<Args &&>(args)...};
        }

    private:
        mutable typename Dist::param_type params_;
    };

    struct DiscretePointMutation : public MutatorBase {
        auto operator()(Turingforge::RandomGenerator &random, Individual individual) const -> Individual override;

        auto Add(Turingforge::Scalar value, Turingforge::Scalar weight = 1.0) -> void {
            values_.push_back(value);
            weights_.push_back(weight);
        }

    private:
        std::vector<Turingforge::Scalar> weights_;
        std::vector<Turingforge::Scalar> values_;
    };

    struct MultiMutation : public MutatorBase {
        auto operator()(Turingforge::RandomGenerator & /*random*/, Individual /*args*/) const -> Individual override;

        void Add(const MutatorBase &op, double prob) {
            operators_.push_back(std::ref(op));
            probabilities_.push_back(prob);
        }

        [[nodiscard]] auto Count() const -> size_t { return operators_.size(); }

    private:
        std::vector<std::reference_wrapper<const MutatorBase>> operators_;
        std::vector<double> probabilities_;
    };

    struct ChangeFunctionMutation : public MutatorBase {
        explicit ChangeFunctionMutation(PrimitiveSet ps)
                : pset_(std::move(ps)) {
        }

        auto operator()(Turingforge::RandomGenerator & /*random*/, Individual /*args*/) const -> Individual override;

    private:
        PrimitiveSet pset_;
    };

    struct RemoveInteractionMutation final : public MutatorBase {
        explicit RemoveInteractionMutation(PrimitiveSet ps) : pset_(std::move(ps)) {}

        auto operator()(Turingforge::RandomGenerator & /*random*/, Individual /*args*/) const -> Individual override;

    private:
        PrimitiveSet pset_;
    };

    struct InsertInteractionMutation final : public MutatorBase {
        InsertInteractionMutation(CreatorBase &creator, CoefficientInitializerBase &coeffInit, size_t maxLength)
                : creator_(creator), coefficientInitializer_(coeffInit), maxLength_(maxLength) {
        }

        auto operator()(Turingforge::RandomGenerator & /*random*/, Individual /*args*/) const -> Individual override;

    private:
        std::reference_wrapper<CreatorBase> creator_;
        std::reference_wrapper<CoefficientInitializerBase> coefficientInitializer_;
        size_t maxLength_;
    };

    struct ReplaceInteractionMutation : public MutatorBase {
        ReplaceInteractionMutation(CreatorBase &creator, CoefficientInitializerBase &coeffInit, size_t maxLength)
                : creator_(creator), coefficientInitializer_(coeffInit), maxLength_(maxLength) {
        }

        auto operator()(Turingforge::RandomGenerator & /*random*/, Individual /*args*/) const -> Individual override;

    private:
        std::reference_wrapper<CreatorBase> creator_;
        std::reference_wrapper<CoefficientInitializerBase> coefficientInitializer_;
        size_t maxLength_;
    };

    struct ShuffleInteractionsMutation : public MutatorBase {
        auto operator()(Turingforge::RandomGenerator & /*random*/, Individual /*args*/) const -> Individual override;
    };
} // namespace Turingforge

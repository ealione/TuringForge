#pragma once

#include "creator.hpp"
#include "turing_forge/core/individual.hpp"

namespace Turingforge {
    struct CoefficientInitializerBase : public OperatorBase<void, Individual &> {
    };

    struct IndividualInitializerBase : public OperatorBase<Individual> {
    };

    template<typename Dist>
    struct CoefficientInitializer : public CoefficientInitializerBase {
        using IndividualCheckCallback = std::function<bool(Turingforge::Individual)>;

        explicit CoefficientInitializer(IndividualCheckCallback callback)
                : callback_(std::move(callback)) {
        }

        CoefficientInitializer()
                : CoefficientInitializer([](auto const &node) { return node.Length != 0; }) {
        }

        auto
        operator()(Turingforge::RandomGenerator &random, Turingforge::Individual &individual) const -> void override {
            for (auto &coefficient: individual.Coefficients) {
                if (callback_(individual)) {
                    coefficient = Dist(params_)(random);
                }
            }
        }

        template<typename... Args>
        auto ParameterizeDistribution(Args... args) const -> void {
            params_ = typename Dist::param_type{std::forward<Args &&>(args)...};
        }

    private:
        mutable typename Dist::param_type params_;
        IndividualCheckCallback callback_;
    };

    template<typename Dist>
    struct IndividualInitializer : public IndividualInitializerBase {
        explicit IndividualInitializer(Turingforge::CreatorBase &creator)
                : creator_(creator) {
        }

        auto operator()(Turingforge::RandomGenerator &random) const -> Turingforge::Individual override {
            return creator_(random, terms_, exponent_limit_, objectives_count_); // initialize individual
        }

        template<typename... Args>
        auto ParameterizeDistribution(Args... args) const -> void {
            params_ = typename Dist::param_type{std::forward<Args &&>(args)...};
        }

        void SetTerms(size_t terms) { terms_ = terms; }

        auto Terms() const -> size_t { return terms_; }

        void SetExponentLimit(size_t exponent_limit) { exponent_limit_ = exponent_limit; }

        auto ExponentLimit() const -> size_t { return exponent_limit_; }

        void SetObjectivesCount(size_t objectives_count) { objectives_count_ = objectives_count; }

        auto ObjectivesCount() const -> size_t { return objectives_count_; }

        void SetCreator(CreatorBase const &creator) { creator_ = creator; }

        [[nodiscard]] auto Creator() const -> CreatorBase const & { return creator_.get(); }

        static constexpr size_t DefaultExponentLimit{100};

    private:
        mutable typename Dist::param_type params_;
        std::reference_wrapper<Turingforge::CreatorBase const> creator_;
        size_t terms_{1};
        size_t exponent_limit_{DefaultExponentLimit};
        size_t objectives_count_{1};
    };

    // wraps a creator and generates trees from a given size distribution
    using UniformCoefficientInitializer = CoefficientInitializer<std::uniform_real_distribution<Turingforge::Scalar>>;
    using NormalCoefficientInitializer = CoefficientInitializer<std::normal_distribution<Turingforge::Scalar>>;

    using UniformIndividualInitializer = IndividualInitializer<std::uniform_int_distribution<size_t>>;
    using NormalindividualInitializer = IndividualInitializer<std::normal_distribution<>>;

} // namespace Turingforge
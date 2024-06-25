#include <algorithm>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <random>
#include <type_traits>

#include "turing_forge/operators/mutation.hpp"
#include "turing_forge/operators/creator.hpp"
#include "turing_forge/operators/initializer.hpp"

namespace Turingforge {

    auto DiscretePointMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        auto& coefficient = individual.Coefficients;
        auto it = Turingforge::Random::Sample(random, coefficient.begin(), coefficient.end());
        ENSURE(it < coefficient.end());

        auto s = std::reduce(weights_.cbegin(), weights_.cend(), Turingforge::Scalar { 0 }, std::plus {});
        auto r = std::uniform_real_distribution<Turingforge::Scalar>(0., s)(random);

        Turingforge::Scalar c { 0 };
        for (auto i = 0UL; i < weights_.size(); ++i) {
            c += weights_[i];
            if (c > r) {
                *it = values_[i];
                break;
            }
        }

        return individual;
    }

    auto MultiMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        auto sum = std::reduce(probabilities_.begin(), probabilities_.end());
        auto r = std::uniform_real_distribution<double>(0, sum)(random);
        auto c = 0.0;
        auto i = 0U;
        for (; i < probabilities_.size(); ++i) {
            c += probabilities_[i];
            if (c > r) {
                break;
            }
        }
        auto op = operators_[i];
        return op(random, std::move(individual));
    }

    auto ChangeFunctionMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        auto& functions = individual.Functions;

        auto it = Turingforge::Random::Sample(random, functions.begin(), functions.end());
        if (it == functions.end()) {
            return individual; // no functions in the individual, nothing to do
        }

        auto minArity = std::min(static_cast<size_t>(it->Arity), pset_.MinimumArity(it->HashValue));
        auto maxArity = std::max(static_cast<size_t>(it->Arity), pset_.MaximumArity(it->HashValue));

        auto n = pset_.SampleRandomSymbol(random, minArity, maxArity);
        it->Type = n.Type;
        it->HashValue = n.HashValue;
        return individual;
    }

    auto ReplaceInteractionMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        auto oldLen = individual.Length + 1U;

        using Signed = std::make_signed<size_t>::type;

        // the correction below is necessary because it can happen that maxLength_ < coefficient.size()
        auto maxLength = static_cast<Signed>(maxLength_ - oldLen);
        maxLength = std::max(maxLength, Signed { 1 });

        auto newLen = std::uniform_int_distribution<Signed>(Signed { 1 }, maxLength)(random);
        auto mutated = creator_(random, static_cast<size_t>(newLen), 1, 1);

        std::copy(mutated.Functions.begin(), mutated.Functions.end(), individual.Functions.begin());
        std::copy(mutated.Coefficients.begin(), mutated.Coefficients.end(), individual.Coefficients.begin());
        std::copy(mutated.Polynomials.begin(), mutated.Polynomials.end(), individual.Polynomials.begin());

        return individual;
    }

    auto RemoveInteractionMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        using Signed = std::make_signed<size_t>::type;

        if (individual.Length == 1) {
            return individual; // nothing to remove
        }
        
        auto index = std::uniform_int_distribution<Signed>(Signed { 1 }, individual.Length - 1)(random);

        individual.Functions.erase(individual.Functions.begin() + index);
        individual.Coefficients.erase(individual.Coefficients.begin() + index);
        individual.Polynomials.erase(individual.Polynomials.begin() + index);

        --individual.Length;

        return Individual{individual};
    }

    auto InsertInteractionMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        if (individual.Length >= maxLength_) {
            // we can't insert anything because the individual length is at the limit
            return individual;
        }

        auto& functions = individual.Functions;
        auto const& creator = creator_.get();
        auto const& pset = creator.GetPrimitiveSet();

        auto test = [&](auto const& function) {
            // TODO:: Arity check not needed for now
            return static_cast<bool>(function.Type & (FunctionType::Log | FunctionType::Exp | FunctionType::Sin | FunctionType::Cos)) && (pset.MaximumArity(function.HashValue) == 1);
        };

        auto n = std::count_if(functions.begin(), functions.end(), test);

        if (n == 0) {
            return individual;
        }

        auto availableLength = maxLength_ - functions.size();
        EXPECT(availableLength > 0);

        auto newLen = std::uniform_int_distribution<size_t>(1, availableLength)(random);

        auto mutated = creator_(random, newLen, 1, 1);
        coefficientInitializer_(random, mutated);

        // copy interactions
        std::copy(individual.Functions.begin(), individual.Functions.end(), std::back_inserter(mutated.Functions));
        std::copy(individual.Coefficients.begin(), individual.Coefficients.end(), std::back_inserter(mutated.Coefficients));
        std::copy(individual.Polynomials.begin(), individual.Polynomials.end(), std::back_inserter(mutated.Polynomials));

        mutated.Length = mutated.Functions.size();

        return mutated;
    }

    auto ShuffleInteractionsMutation::operator()(Turingforge::RandomGenerator& random, Individual individual) const -> Individual
    {
        // shuffle child indices
        std::shuffle(individual.Polynomials.begin(), individual.Polynomials.end(), random);

        return individual;
    }
} // namespace Turingforge
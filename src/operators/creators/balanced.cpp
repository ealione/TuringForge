#include "turing_forge/operators/creator.hpp"
#include "turing_forge/core/primitive_set.hpp"
#include "turing_forge/core/individual.hpp"
#include "turing_forge/core/variable.hpp"

namespace Turingforge {

    // Create candidate solutions formed as linear combinations of monomial functions with integer exponents.
    // TODO: remove default argument from on override method!
    auto BalancedCreator::operator()(Turingforge::RandomGenerator& random, size_t terms, Turingforge::Scalar exponent_limit, size_t nObj = 1) const -> Individual
    {
        auto const& pset = GetPrimitiveSet();
        auto const& variables = GetVariables();

        auto maxArity = 1, minArity = 1;

        Individual individual;
        individual.Length = static_cast<uint16_t>(terms);
        individual.Fitness = Vector<Scalar>(nObj, std::numeric_limits<Scalar>::max());
        individual.Coefficients.resize(terms, 1.0);
        individual.Functions.resize(terms);
        std::generate(individual.Functions.begin(), individual.Functions.end(),
                      [&]() { return pset.SampleRandomSymbol(random, static_cast<size_t>(minArity), static_cast<size_t>(maxArity)); });
        individual.Polynomials.resize(terms);
        for (auto &poly: individual.Polynomials) {
            poly.resize(variables.size());
            std::generate(poly.begin(), poly.end(), [&]() {
                return static_cast<int>(static_cast<Scalar>(Turingforge::Random::Uniform(random, 0.0F, +1.0F)) * 2 * (exponent_limit + 1) - 1 - exponent_limit);
            });
        }

        return individual;
    }

}